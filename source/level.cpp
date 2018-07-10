#include "level.h"

int levelWidth;
int levelHeight;

BYTE *levelMap = nullptr;
BYTE *levelTileAttributes = nullptr;

BYTE backgroundScroll;

int backgroundEffect = 0;

//NPC Functions
void updateNPC()
{
	//Remove stuff
	for (size_t i = npcs.size() - 1; i > 0; i--) {
		if (!(npcs[i].cond & npccond_alive))
			npcs.erase(npcs.begin() + i);
	}

	//Update
	for (unsigned int i = 0; i < npcs.size(); i++) {
		if (npcs[i].cond & npccond_alive)
		{
			npcs[i].update();
			npcHitMap(i);
		}
	}
}

void drawNPC()
{
	//Draw
	for (unsigned int i = 0; i < npcs.size(); i++) {
		npcs[i].draw();
	}
}

//Get stage data
BYTE *stageData = nullptr;
char stblPath[256] = "data/stage.tbl";

int stblSize = loadFile(stblPath, &stageData);

BYTE getTileAttribute(int x, int y) {
	if (x >= 0 && x < levelWidth && y >= 0 && y < levelHeight)
		return levelTileAttributes[levelMap[(x + y * levelWidth)]];

	return 0;
}

void createNpc(int x, int y, int direction, int xsp, int ysp, short flag, short event, short type, short flags)
{
	npc newNPC;
	newNPC.init(x, y, flag, event, type, flags);

	newNPC.direction = direction;
	newNPC.xsp = xsp;
	newNPC.ysp = ysp;

	npcs.push_back(newNPC);
}

void loadLevel(int levelIndex) {
	if (stblSize < 0)
		doCustomError("stage.tbl failed to load");

	//Get stage's stage.tbl offset
	int tbloffset = levelIndex * 0xE5;

	//Get tileset filename
	char* tilesetFilename = (char*)&stageData[tbloffset];

	//Get filename
	char* levelFilename = (char*)&stageData[tbloffset+0x20];

	//Get background stuff
	backgroundScroll = readLElong(stageData, tbloffset + 0x40);

	char* backgroundFilename = (char*)&stageData[tbloffset + 0x44];

	//Npc sprite sheets
	char* npcSheet1 = (char*)&stageData[tbloffset + 0x64];
	char* npcSheet2 = (char*)&stageData[tbloffset + 0x84];

	//Load pxm
	char pxmPath[256];
	snprintf(pxmPath, 256, "data/Stage/%s.pxm", levelFilename);

	BYTE *pxm = nullptr;
	int pxmSize = loadFile(pxmPath, &pxm);
	if (pxmSize < 0 ) { doCustomError("Couldn't read pxm."); doCustomError(pxmPath); }
	
	levelWidth = readLEshort(pxm, 4);
	levelHeight = readLEshort(pxm, 6);

	if (levelMap)
		delete levelMap;

	levelMap = new BYTE[pxmSize-8];

	for (int i = 8; i < pxmSize; i++) {
		levelMap[i - 8] = pxm[i];
	}

	//DONE WITH PXM
	free(pxm);

	//Load pxa
	char pxaPath[256];
	snprintf(pxaPath, 256, "data/Stage/%s.pxa", tilesetFilename);

	BYTE *pxa = nullptr;
	int pxaSize = loadFile(pxaPath, &pxa);
	if (pxaSize < 0) { doCustomError("Couldn't read pxa."); doCustomError(pxaPath); }

	if (levelTileAttributes)
		delete levelTileAttributes;
	levelTileAttributes = new BYTE[pxaSize];
	
	for (int i = 0; i < pxaSize; i++) {
		levelTileAttributes[i] = pxa[i];
	}

	//DONE WITH PXA
	free(pxa);

	//Load pxe
	char pxePath[256];
	snprintf(pxePath, 256, "data/Stage/%s.pxe", levelFilename);

	BYTE *pxe = nullptr;
	int pxeSize = loadFile(pxePath, &pxe);
	if (pxeSize < 0) { doCustomError("Couldn't read pxe."); doCustomError(pxePath); }

	//Clear old npcs
	npcs.clear();
	npcs.shrink_to_fit();

	//Load npcs
	int entityCount = (int)pxe[4];

	for (int i = 0; i < entityCount; i++) {
		int offset = (i * 12) + 8;

		short x = readLEshort(pxe, offset);
		short y = readLEshort(pxe, offset + 2);
		short flag = readLEshort(pxe, offset + 4);
		short event = readLEshort(pxe, offset + 6);
		short type = readLEshort(pxe, offset + 8);
		short flags = readLEshort(pxe, offset + 10);

		bool create = true;

		if (flags & npc_appearset) {
			if (getFlag(flag) == false) { create = false; }
		}

		if (flags & npc_hideset) {
			if (getFlag(flag) == true) { create = false; }
		}

		if (create == true)
		{
			npc newNPC;
			newNPC.init(x << 13, y << 13, flag, event, type, flags);
			npcs.push_back(newNPC);
		}
	}

	//Load tileset
	char tileImagePath[256];
	snprintf(tileImagePath, 256, "data/Stage/Prt%s.bmp", tilesetFilename);

	loadBMP(tileImagePath, &sprites[0x02]);

	//Load background
	char bgImagePath[256];
	snprintf(bgImagePath, 256, "data/%s.bmp", backgroundFilename);

	loadBMP(bgImagePath, &sprites[0x1C]);

	//Load npc sheets
	//Load sheet 1
	char npcSheet1Path[256];
	snprintf(npcSheet1Path, 256, "data/Npc/Npc%s.bmp", npcSheet1);

	loadBMP(npcSheet1Path, &sprites[0x15]);

	//Load sheet 2
	char npcSheet2Path[256];
	snprintf(npcSheet2Path, 256, "data/Npc/Npc%s.bmp", npcSheet2);
	strcpy(npcSheet2Path, "data/Npc/Npc");
	strcat(npcSheet2Path, npcSheet2);
	strcat(npcSheet2Path, ".bmp");

	loadBMP(npcSheet2Path, &sprites[0x16]);

	//Load tsc script
	char tscPath[256];
	snprintf(tscPath, 256, "data/Stage/%s.tsc", levelFilename);

	loadTsc(tscPath);

	//END
	return;
}

void drawLevel(bool foreground) {
	if (foreground == false) { //Draw background
		RECT rect;

		int skyOff;

		int w, h;
		SDL_QueryTexture(sprites[0x1C], NULL, NULL, &w, &h);

		rect = { 0, 0, w, h };

		//Update background effect
		if (backgroundScroll == 5)
		{
			backgroundEffect += 0xC00;
		}

		else if (backgroundScroll >= 5 && backgroundScroll <= 7)
		{
			++backgroundEffect;
			backgroundEffect %= (w * 2);
		}

		switch (backgroundScroll)
		{
			case 0:
				for (int x = 0; x < screenWidth; x += w)
				{
					for (int y = 0; y < screenHeight; y += h)
						drawTextureFromRect(sprites[0x1C], &rect, x, y, true);
				}

				break;

			case 1:
				for (int x = -(viewX / 0x400 % w); x < screenWidth; x += w)
				{
					for (int y = -(viewY / 0x400 % h); y < screenHeight; y += h)
						drawTextureFromRect(sprites[0x1C], &rect, x << 9, y << 9, true);
				}

				break;

			case 2:
				for (int x = -(viewX / 0x200 % w); x < screenWidth; x += w)
				{
					for (int y = -(viewY / 0x200 % h); y < screenHeight; y += h)
						drawTextureFromRect(sprites[0x1C], &rect, x << 9, y << 9, true);
				}

				break;

			case 6: case 7:
				//Draw sky
				rect = { 0, 0, w, 88 };

				skyOff = (((w - screenWidth) / 2) << 9);
				
				drawTextureFromRect(sprites[0x1C], &rect, -skyOff, 0, true);

				//Cloud layers
				rect.top = 88;
				rect.bottom = 123;
				for (int i = 0; i <= (screenWidth / w) + 1; ++i)
					drawTextureFromRect(sprites[0x1C], &rect, ((w * i) - ((backgroundEffect / 2) % w)) << 9, rect.top << 9, true);

				rect.top = 123;
				rect.bottom = 146;
				for (int i = 0; i <= (screenWidth / w) + 1; ++i)
					drawTextureFromRect(sprites[0x1C], &rect, ((w * i) - (backgroundEffect % w)) << 9, rect.top << 9, true);

				rect.top = 146;
				rect.bottom = 176;
				for (int i = 0; i <= (screenWidth / w) + 1; ++i)
					drawTextureFromRect(sprites[0x1C], &rect, ((w * i) - ((backgroundEffect * 2) % w)) << 9, rect.top << 9, true);

				rect.top = 176;
				rect.bottom = 240;
				for (int i = 0; i <= (screenWidth / w) + 1; ++i)
					drawTextureFromRect(sprites[0x1C], &rect, ((w * i) - ((backgroundEffect * 4) % w)) << 9, rect.top << 9, true);

				break;

			default:
				break;
		}
	}

	//Render tiles
	int xFrom = clamp((viewX + 0x1000) >> 13, 0, levelWidth);
	int xTo = clamp((((viewX + 0x1000) + (screenWidth << 9)) >> 13) + 1, 0, levelWidth); //add 1 because edge wouldn't appear

	int yFrom = clamp((viewY + 0x1000) >> 13, 0, levelHeight);
	int yTo = clamp((((viewY + 0x1000) + (screenHeight << 9)) >> 13) + 1, 0, levelHeight); //add 1 because edge wouldn't appear

	for (int x = xFrom; x < xTo; x++) {
		for (int y = yFrom; y < yTo; y++) {
			int i = x + y * levelWidth;

			int tile = levelMap[i];
			if (tile) {
				int attribute = levelTileAttributes[tile];

				if ((attribute < 0x20 && foreground == false) || (attribute >= 0x40 && foreground == true))
				{
					int drawX = i % levelWidth;
					int drawY = i / levelWidth;

					switch (attribute)
					{
						case(0x43):
							ImageRect.x = 256;
							ImageRect.y = 48;
							ImageRect.w = 16;
							ImageRect.h = 16;

							drawTexture(sprites[0x14], (drawX << 13) - 0x1000, (drawY << 13) - 0x1000, false);

							continue;

						default:
							ImageRect.x = (tile % 16) * 16;
							ImageRect.y = (tile / 16) * 16;
							ImageRect.w = 16;
							ImageRect.h = 16;

							drawTexture(sprites[0x02], (drawX << 13) - 0x1000, (drawY << 13) - 0x1000, false);

							continue;
					}
				}
			}
		}
	}

	//Render black bars in foreground
	if (foreground)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		if (viewX < 0) //Typically won't be out of bounds unless centred to stage
		{
			drawRect(0, 0, -viewX, screenHeight << 9);
			drawRect((screenWidth << 9) + viewX, 0, -viewX, screenHeight << 9);
		}

		if (viewY < 0)
		{
			drawRect(0, 0, screenWidth << 9, -viewY);
			drawRect(0, (screenHeight << 9) + viewY, screenWidth << 9, -viewY);
		}
	}
}