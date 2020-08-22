
#include "dce.h"

#include <stdio.h>
#include <string.h>


static DCE_Trigger   DCE_TRIGGER[DCE_MAX_TRIGGERS];
static unsigned char DCE_TRIGGERS = 0;

void DCE_TriggerReset(unsigned char index)
{
	DCE_TriggerResetRBO(index);
	DCE_TriggerResetCBO(index);
}

void DCE_TriggerInit(char * trigger, unsigned char index)
{
	DCE_TriggerInitRBO(trigger, index);	
	DCE_TriggerInitCBO(trigger, index);	
}

void DCE_TriggerCallback(vec3f_t * O, unsigned char index)
{
	float d;
    unsigned char i;

    for(i = 0; i < DCE_TRIGGERS; i++)
    {
        vec3f_distance(O->x, O->y, O->z, DCE_TRIGGER[i].P.x, DCE_TRIGGER[i].P.y, DCE_TRIGGER[i].P.z, d);
            
        if(d < DCE_TRIGGER_DELTA)
        {
            DCE_TriggerCBO(DCE_TRIGGER[i].segment, DCE_TRIGGER[i].trigger, index);
		    DCE_TriggerRBO(DCE_TRIGGER[i].segment, DCE_TRIGGER[i].trigger, index);
		    DCE_TriggerDOOR(DCE_TRIGGER[i].segment, DCE_TRIGGER[i].trigger);
		}
	}
}

void DCE_AddTrigger(vec3f_t * P, unsigned char trigger, unsigned char active, char * segment)
{
	DCE_TRIGGER[DCE_TRIGGERS].active = active;
	DCE_TRIGGER[DCE_TRIGGERS].trigger = trigger;	
	
	vec3f_copy(P, &DCE_TRIGGER[DCE_TRIGGERS].P);
	
	strcpy(DCE_TRIGGER[DCE_TRIGGERS].segment, segment);
	
	++DCE_TRIGGERS;
}

inline void DCE_ReadVec3F(char * buf, float * x, float * y, float * z)
{
	char id[64];
	sscanf(buf, "%s %c %c %f %c %f %c %f", id, id, id, x, id, z, id, y);  // Parse Segment Name and Active State
	
	*x *= -10;
	*y *= 10;
	*z *= 10;
}

void DCE_LoadTriggerCluster(char * fname)
{
	FILE * f = NULL;      // Input TXT File
	
	char buf[96]; // Read Buf
	char id[64];  // Tmp Buf
    char segment[32];
    int active;
    vec3f_t V;

	f = fopen(fname, "rb");  // Open Segement Descriptor File
	if(f == NULL)
	{
		printf("File Error %s\n", fname);
		return;
	}
	
	fgets(buf, 96, f);       // Read First Line
	
	sscanf(buf, "%s %s %i", id, segment, &active);  // Parse Segment Name and Active State
	
	while(fgets(buf, 64, f)) // Scan to Render Objects
		if(buf[0] == 'T')
            break;
    
    if(buf[0] != 'T') // NO TRIGGER Found
    {
        fclose(f);    
		return;       
	}
	        
    fgets(buf, 64, f);       // Skip { brace start

	while(fgets(buf, 64, f)) // Scan past open brace
	{
		if(buf[0] == '}')
            break;  
		
		switch(buf[5])
		{
			case 'n':
				DCE_ReadVec3F(buf, &V.x, &V.y, &V.z);

				DCE_AddTrigger(&V, 1, active & 0xFF, segment);
				
				break;
				
			case 'f':
                DCE_ReadVec3F(buf, &V.x, &V.y, &V.z);

				DCE_AddTrigger(&V, 0, active & 0xFF, segment);
						
				break;
		}
	}
	
	fclose(f);
}


void DCE_TriggerLobbyA(DCE_Player * player, unsigned char index)
{
    DCE_TriggerReset(index); 
	DCE_TriggerInit("STAIRS", index);
	DCE_TriggerInit("EXTERIOR", index);
	DCE_TriggerInit("LOBBY", index);
	DCE_TriggerInit("LOBBY_INSIDE", index);
	
    player->position.x = -471.7;
    player->position.y = -315.8;
    player->position.z = -189.4;

    player->lookAt.x = -461.7;
    player->lookAt.y = -315.8;
    player->lookAt.z = -189.4;
}

void DCE_TriggerLobbyB(DCE_Player * player, unsigned char index)
{
    DCE_TriggerReset(index); 
	DCE_TriggerInit("STAIRS", index);
	DCE_TriggerInit("EXTERIOR", index);
	DCE_TriggerInit("LOBBY", index);
	DCE_TriggerInit("LOBBY_INSIDE", index);
	
    player->position.x = -456.1;
    player->position.y = -65.3;
    player->position.z = 42.2;

    player->lookAt.x = -456.1;
    player->lookAt.y = -65.3;
    player->lookAt.z = 32.2;
}

void DCE_TriggerRoof(DCE_Player * player, unsigned char index)
{
    DCE_TriggerReset(index); 
	DCE_TriggerInit("ROOF", index);
	DCE_TriggerInit("STAIRS2", index);
	DCE_TriggerInit("EXTERIOR", index);
	
    player->position.x = -103;
    player->position.y = 185;
    player->position.z = -59;

    player->lookAt.x = -103;
    player->lookAt.y = 185;
    player->lookAt.z = -69;
}

void DCE_TriggerVaultA(DCE_Player * player, unsigned char index)
{
    DCE_TriggerReset(index); 
	DCE_TriggerInit("STAIRS", index);	
	DCE_TriggerInit("VAULT", index);

    player->position.x = -402.5;
    player->position.y = -441.0;
    player->position.z = -663.3;

    player->lookAt.x = -392.5;
    player->lookAt.y = -441.0;
    player->lookAt.z = -663.3;
}

void DCE_TriggerVaultB(DCE_Player * player, unsigned char index)
{
    DCE_TriggerReset(index); 
	DCE_TriggerInit("STAIRS", index);	
	DCE_TriggerInit("VAULT", index);

    player->position.x = -481.5;
    player->position.y = -441.0;
    player->position.z = -1093.6;

    player->lookAt.x = -471.5;
    player->lookAt.y = -441.0;
    player->lookAt.z = -1093.6;
}

void DCE_TriggerFloor2A(DCE_Player * player, unsigned char index)
{
    DCE_TriggerReset(index); 
	DCE_TriggerInit("STAIRS", index);	
	DCE_TriggerInit("STAIRS2", index);
	DCE_TriggerInit("2NDFLOOR", index);
	DCE_TriggerInit("EXTERIOR", index);
	
    player->position.x = -134.1;
    player->position.y = 60;
    player->position.z = -156;

    player->lookAt.x = -124.1;
    player->lookAt.y = 60;
    player->lookAt.z = -156;
}


void DCE_TriggerFloor2B(DCE_Player * player, unsigned char index)
{
    DCE_TriggerReset(index); 
	DCE_TriggerInit("STAIRS", index);	
	DCE_TriggerInit("STAIRS2", index);
	DCE_TriggerInit("2NDFLOOR", index);
	DCE_TriggerInit("EXTERIOR", index);
	
    player->position.x = -103.1;
    player->position.y = 60;
    player->position.z = -462.2;

    player->lookAt.x = -103.1;
    player->lookAt.y = 60;
    player->lookAt.z = -472.2;
}

static unsigned char rnd;

void DCE_ReSpawn(DCE_Player * player, unsigned char index)
{
	DCE_InitPlayerState(player); 
	
	rnd = (rand() % 8);
	
	switch(rnd)
	{
		case 0:
			DCE_TriggerLobbyA(player, index);
			break;

		case 1:
			DCE_TriggerLobbyB(player, index);
			break;
			
		case 2:
			DCE_TriggerVaultA(player, index);
			break;			
			
		case 3:
			DCE_TriggerVaultB(player, index);
			break;					
			
		case 5:
			DCE_TriggerFloor2A(player, index);
			break;			
			
		case 6:
			DCE_TriggerFloor2B(player, index);
			break;				

		case 7:
			DCE_TriggerRoof(player, index);
			break;				
	}
}

void DCE_ReSpawn2(DCE_Player * player, unsigned char index)
{
	DCE_InitPlayerState(player); 
	
	unsigned char r = (rand() % 8);
	
	while(r == rnd)
	    r = (rand() % 8);
	    
	switch(r)
	{
		case 0:
			DCE_TriggerLobbyA(player, index);
			break;

		case 1:
			DCE_TriggerLobbyB(player, index);
			break;
			
		case 2:
			DCE_TriggerVaultA(player, index);
			break;			
			
		case 3:
			DCE_TriggerVaultB(player, index);
			break;					
			
		case 5:
			DCE_TriggerFloor2A(player, index);
			break;			
			
		case 6:
			DCE_TriggerFloor2B(player, index);
			break;				

		case 7:
			DCE_TriggerRoof(player, index);
			break;				
	}
}
