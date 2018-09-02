#include "shootFunctions.h"
#include "bullet.h"
#include "sound.h"
#include "player.h"
#include "input.h"
#include "caret.h"

void launchMissile(int level)
{
	int bulletId = 0;
	int shootDirect = 0;
	int shootX = 0;
	int shootY = 0;
	int caretX = 0;

	if (isKeyPressed(keyShoot))
	{
		switch (level)
		{
		case(1):
			if (weaponBullets(bullet_MissileLauncherLevel1) >= 1) { return; }
			else if (useWeaponAmmo(1) ^ 1)
			{
				playSound(SFX_NoAmmo);
				createCaret(currentPlayer.x, currentPlayer.y, 16, 0);
				return;
			}
			bulletId = bullet_MissileLauncherLevel1;
			break;
		case(2):
			if (weaponBullets(bullet_MissileLauncherLevel1) >= 1) { return; }
			else if (useWeaponAmmo(1) ^ 1)
			{
				playSound(SFX_NoAmmo);
				createCaret(currentPlayer.x, currentPlayer.y, 16, 0);
				return;
			}
			bulletId = bullet_MissileLauncherLevel2;
			break;
		case(3):
			if (weaponBullets(bullet_MissileLauncherLevel1) >= 3) { return; }
			else if (useWeaponAmmo(1) ^ 1)
			{
				playSound(SFX_NoAmmo);
				createCaret(currentPlayer.x, currentPlayer.y, 16, 0);
				return;
			}
			bulletId = bullet_MissileLauncherLevel3;
			break;
		}

		if (currentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (currentPlayer.direct - 1) * 0x800;
			caretX = (currentPlayer.direct - 1) * 0x800;
			shootY = -0x1000;
		}
		else if (currentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (currentPlayer.direct - 1) * 0x800;
			caretX = (currentPlayer.direct - 1) * 0x800;
			shootY = 0x1000;
		}
		else
		{
			shootDirect = currentPlayer.direct;
			shootX = (currentPlayer.direct - 1) * 0xC00;
			caretX = (currentPlayer.direct - 1) * 0x1800;
			shootY = 0x600;
		}

		playSound(SFX_PolarStarShootLevel12);
		if (level < 3)
		{
			createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
		}
		else
		{
			createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
			if (shootDirect == dirLeft || shootDirect == dirRight)
			{
				createBullet(bulletId, currentPlayer.x + 1536, currentPlayer.y, shootDirect);
				createBullet(bulletId, currentPlayer.x - 1536, currentPlayer.y, shootDirect);
			}
			else if (shootDirect == dirUp || shootDirect == dirDown)
			{
				createBullet(bulletId, currentPlayer.x, currentPlayer.y + 1536, shootDirect);
				createBullet(bulletId, currentPlayer.x, currentPlayer.y - 1536, shootDirect);
			}

		}
		createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star, 0);
	}

	return;
}

void superMissileLaunch(int level)
{
	int bulletId = 0;
	int shootDirect = 0;
	int shootX = 0;
	int shootY = 0;
	int caretX = 0;

	if (isKeyPressed(keyShoot))
	{
		if (useWeaponAmmo(1) ^ 1)
		{
			playSound(SFX_NoAmmo);
			createCaret(currentPlayer.x, currentPlayer.y, 16, 0);
			return;
		}
		switch (level)
		{
		case(1):
			if (weaponBullets(4) > 1) { return; }
			bulletId = bullet_SuperMissileLauncherLevel3;
			break;
		case(2):
			if (weaponBullets(4) > 1) { return; }
			bulletId = bullet_SuperMissileLauncherLevel3;
			break;
		case(3):
			if (weaponBullets(4) > 3) { return; }
			bulletId = bullet_SuperMissileLauncherLevel3;
			break;
		}

		if (currentPlayer.up)
		{
			shootDirect = dirUp;
			shootX = (currentPlayer.direct - 1) * 0x800;
			caretX = (currentPlayer.direct - 1) * 0x800;
			shootY = -0x1000;
		}
		else if (currentPlayer.down)
		{
			shootDirect = dirDown;
			shootX = (currentPlayer.direct - 1) * 0x800;
			caretX = (currentPlayer.direct - 1) * 0x800;
			shootY = 0x1000;
		}
		else
		{
			shootDirect = currentPlayer.direct;
			shootX = (currentPlayer.direct - 1) * 0xC00;
			caretX = (currentPlayer.direct - 1) * 0x1800;
			shootY = 0x600;
		}

		playSound(SFX_PolarStarShootLevel12);
		createBullet(bulletId, currentPlayer.x + shootX, currentPlayer.y + shootY, shootDirect);
		createCaret(currentPlayer.x + caretX, currentPlayer.y + shootY, effect_Star, 0);
	}
	return;
}