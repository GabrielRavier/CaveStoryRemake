#include "npc.h"
#include "npcAct.h"

BYTE *npcAttributes = nullptr;
char npcTblPath[256] = "data/npc.tbl";

int npcTblSize = loadFile(npcTblPath, &npcAttributes);

int npcAmmount = npcTblSize / 24;

std::vector<npc> npcs(0);

int getNPCAttribute(UINT npcId, UINT offset, UINT offset2, UINT offsetSize, UINT returnSize)
{
	int absOffset = (offset * npcAmmount) + (offsetSize * npcId) + offset2;

	switch (returnSize) {
		case(1):
			return npcAttributes[absOffset];
			break;

		case(2):
			return readLEshort(npcAttributes, absOffset);
			break;

		case(4):
			return readLElong(npcAttributes, absOffset);
			break;

		default:
			doCustomError("INVALID getNPCAttribute RETURNSIZE VALUE");
			return -1;
			break;
	}
}

void npc::init(int spawnX, int spawnY, short flagId, short eventId, short npcType, short npcFlags)
{
	memset(this, 0, sizeof(*this));

	x = spawnX;
	y = spawnY;
	flag = flagId;
	event = eventId;
	type = npcType;
	flags = getNPCAttribute(npcType, 0, 0, 2, 2) | npcFlags;

	cond = npccond_alive;

	collideRect.left = getNPCAttribute(npcType, 16, 0, 4, 1) << 9;
	collideRect.top = getNPCAttribute(npcType, 16, 1, 4, 1) << 9;
	collideRect.right = getNPCAttribute(npcType, 16, 2, 4, 1) << 9;
	collideRect.bottom = getNPCAttribute(npcType, 16, 3, 4, 1) << 9;

	offset.x = getNPCAttribute(npcType, 20, 0, 4, 1) << 9;
	offset.y = getNPCAttribute(npcType, 20, 1, 4, 1) << 9;

	spriteSheet = getNPCAttribute(npcType, 4, 0, 1, 1);

	damage = getNPCAttribute(npcType, 12, 0, 4, 4);

	return;
}

void npc::update()
{
	npcActs[type](this);

	if (shock) { --shock; }
}

void npc::draw()
{
	if (cond & npccond_alive)
	{
		drawTextureFromRect(sprites[spriteSheet], &frameRect, x - offset.x, y - offset.y, false);

		/*/ COLLISION DEBUG
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

		drawRect((x - collideRect.left - viewX) >> 9, (y - collideRect.top - viewY) >> 9, (collideRect.left + collideRect.right) >> 9, (collideRect.top + collideRect.bottom) >> 9);
		*/
	}
}