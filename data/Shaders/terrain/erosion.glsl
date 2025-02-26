#version 430

#define dropletsNUM 100

layout (local_size_x = dropletsNUM, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D terrainPatch;

uniform int numDrops;
uniform int numSteps;
uniform float inertia;
uniform float gravityForce;
uniform float evaporate;
uniform float startingSpeed;
uniform float initialWater;
float erodeSpeed;
float depositSpeed;


//================================================================================
layout (binding = 1) uniform rainData{
	ivec2[dropletsNUM] droplets;
} rain;

//=================================================================================
struct Droplet
{
  vec2 position;
  vec2 direction;
  float velocity;
  float water;
  float sediment;
};

struct BrushData
{
	ivec2 offset;
	float weight;
};

//=================================================================================
BrushData[3*3] brush;



float gaussian( float x, float mu, float sigma ) {
    float a = ( x - mu ) / sigma;
    return exp( -0.5 * a * a );
}

void GenerateGaussianBrush(int kernelRadius){

	// brush[0] = BrushData(ivec2(0,0), 4.0/16.0); // center
	// // sides
	// brush[1] = BrushData(ivec2(-1,0), 2.0/16.0);
	// brush[2] = BrushData(ivec2(1,0), 2.0/16.0);
	// brush[3] = BrushData(ivec2(0,-1), 2.0/16.0);
	// brush[4] = BrushData(ivec2(0,1), 2.0/16.0);
	// // corners
	// brush[5] = BrushData(ivec2(1,1), 1.0/16.0);
	// brush[6] = BrushData(ivec2(-1,-1), 1.0/16.0);
	// brush[7] = BrushData(ivec2(1,-1), 1.0/16.0);
	// brush[8] = BrushData(ivec2(-1,1), 1.0/16.0);

  float sigma = kernelRadius/2.;
  float sum = 0;
  int rowColSize = kernelRadius*2+1;
  // compute values
  for (int row = 0; row < rowColSize; row++)
    for (int col = 0; col < rowColSize; col++) {
      float x = gaussian(row, kernelRadius, sigma)
               * gaussian(col, kernelRadius, sigma);
      brush[row*rowColSize + col] = BrushData(ivec2(row - kernelRadius, col - kernelRadius),x);
      sum += x;
    }
  // normalize
  for (int row = 0; row < rowColSize; row++)
    for (int col = 0; col < rowColSize; col++)
      brush[row*rowColSize + col].weight /= sum;

}

//=================================================================================
vec3 GetGradientAndHeight(vec2 posf){
	const ivec3 off = ivec3(-1,0,1);
	const ivec2 pos = ivec2(posf.x, posf.y);
	float x = posf.x - pos.x;
	float y = posf.y - pos.y;
	
	vec4 wave = imageLoad(terrainPatch, pos);
	float s11 = wave.x;
    float NW = imageLoad(terrainPatch, pos+ off.xx).x;//[-1,0]
    float NE = imageLoad(terrainPatch, pos+ off.zx).x;//[1,0]
    float SW = imageLoad(terrainPatch, pos+ off.xz).x;//[0,1]
    float SE = imageLoad(terrainPatch, pos+ off.zz).x;//[1,1]

	float gradX = (NE-NW)*(1-x) + (SE-SW)*x;
	float gradY = (SW-NW)*(1-y) + (SE-NE)*y;

    float height = NW * (1 - x) * (1 - y) + NE * x * (1 - y) + SW * (1 - x) * y + SE * x * y;

    return vec3(gradX, gradY, s11);
}


void DepositSediment(vec2 posf, float amountToDeposit)
{
	const ivec3 off = ivec3(-1,0,1);
	const ivec2 pos = ivec2(posf.x, posf.y);
	float offsetInCellx = posf.x - pos.x;
	float offsetInCelly = posf.y - pos.y;

	vec4 NWc = imageLoad(terrainPatch, pos+ off.yy);
    vec4 NEc = imageLoad(terrainPatch, pos+ off.yz);//[1,0]
    vec4 SWc = imageLoad(terrainPatch, pos+ off.zy);//[0,1]
    vec4 SEc = imageLoad(terrainPatch, pos+ off.zz);//[1,1]

    float NW = NWc.x;//[-1,0]
    float NE = NEc.x;//[1,0]
    float SW = SWc.x;//[0,1]
    float SE = SEc.x;//[1,1]

    float partitionToDeposit = amountToDeposit / 4;

    NW += partitionToDeposit * (1 - offsetInCellx) * (1 - offsetInCelly);
    NE += partitionToDeposit * offsetInCellx * (1 - offsetInCelly);
    SW += partitionToDeposit * (1 - offsetInCellx) * offsetInCelly;
    SE += partitionToDeposit * offsetInCellx * offsetInCelly;

    imageStore(terrainPatch, pos+ off.yy, vec4(NW, NWc.y, NWc.z, 1));
    imageStore(terrainPatch, pos+ off.yz, vec4(NE, NEc.y, NEc.z, 1));
    imageStore(terrainPatch, pos+ off.zy, vec4(SW, SWc.y, SWc.z, 1));
    imageStore(terrainPatch, pos+ off.zz, vec4(SE, SEc.y, SEc.z, 1));
}

//=================================================================================
void main()
{
	ivec2 mapSize = imageSize(terrainPatch);

	GenerateGaussianBrush(1);

	uint posX = gl_GlobalInvocationID.x;
	// simplest way to limit number of droplets I found
	if(posX >= numDrops){
		return;
	}

	erodeSpeed = 0.3;
	depositSpeed = 0.3;

    Droplet drop;
    ivec2 posI = rain.droplets[posX];
    drop.position = vec2(posI.x, posI.y);
    drop.direction = vec2(0.0, 0.0);
    drop.velocity = startingSpeed;// starting speed
    drop.water = initialWater; // starting water
    drop.sediment = 0.0f;// cause it doesnt took any sediment yet

    // should be parametrisable
    float sedimentCapacityFactor = 3;
    float minSedimentCapacity = 0.01;

    for(int i = 0;i< numSteps; ++i)
    {
		const ivec2 ipos = ivec2(drop.position.x, drop.position.y);
		float offsetInCellx = drop.position.x - ipos.x;
		float offsetInCelly = drop.position.y - ipos.y;
		const vec3 GradientAndGeight = GetGradientAndHeight(drop.position);
		vec2 positionBak = drop.position;

		drop.direction.x = (drop.direction.x * inertia - GradientAndGeight.x * (1 - inertia));
        drop.direction.y = (drop.direction.y * inertia - GradientAndGeight.y * (1 - inertia));

		if(length(drop.direction)<0.00001)
		{
			break;
		}

        drop.direction = normalize(drop.direction); 

		drop.position += drop.direction;
        float moveDistance = length(positionBak - drop.position);

        int border = 15;

    	if(drop.position.x < border || drop.position.y < border || drop.position.x >= mapSize.x - border || drop.position.y >= mapSize.y - border)
    	{
    		break;
    	}

        float newHeight = GetGradientAndHeight (drop.position).z;
        float deltaHeight = newHeight - GradientAndGeight.z;

        float sedimentCapacity = max(-deltaHeight * drop.velocity * drop.water * sedimentCapacityFactor, minSedimentCapacity);

        bool depositB = false;

        // flowing uphill
        if(deltaHeight > 0 || drop.sediment > sedimentCapacity)
        {
			float amountToDeposit = (deltaHeight > 0) ? min(deltaHeight, drop.sediment) : (drop.sediment - sedimentCapacity) * depositSpeed;
            drop.sediment -= amountToDeposit;

            depositB = true;

            DepositSediment(positionBak, amountToDeposit);
    	}
    	else
    	{
			float amountToErode = min ((sedimentCapacity - drop.sediment) * erodeSpeed, -deltaHeight);
			

			for(int i = 0; i< brush.length();++i)
			{
				float deltaToErode = amountToErode*brush[i].weight;
				ivec2 pos = ivec2(positionBak) + brush[i].offset;

				vec4 heightVal =imageLoad(terrainPatch, pos); 
				float val = heightVal.x;

				float deltaSediment = 0.0f;
				if(val < deltaToErode)
				{
					deltaSediment = val;
				}
				else
				{
					deltaSediment = deltaToErode;
				}

				float result = val - deltaSediment;
				imageStore(terrainPatch, pos, vec4(result, heightVal.y, heightVal.z, heightVal.a));
				drop.sediment += deltaSediment;
			}
    	}

    	// statistics debug
		vec4 val = imageLoad(terrainPatch, ivec2(positionBak));
		imageStore(terrainPatch, ivec2(positionBak), vec4(val.x, sedimentCapacity, val.z + drop.water, 1));

        drop.velocity = sqrt (max(0,drop.velocity * drop.velocity + deltaHeight * gravityForce));
        drop.water *= (1 - evaporate);
        

	}

}  