//----------------------------------------------------------------------------------------------------------------------------------------
//		Knite Guardian
//----------------------------------------------------------------------------------------------------------------------------------------
//		small game idea to test out how well I can make a game via Javidx9's Pixel Game Engine
//----------------------------------------------------------------------------------------------------------------------------------------
#define OLC_PGE_APPLICATION
#include "../../olcPixelGameEngine 2.25.h"
#include <iostream>

class KniteGuard : public olc::PixelGameEngine
{
public:
	KniteGuard()
	{
		sAppName = "Knite Guardian";
	}

public:
	// prepare variables ---------------------------------------------------------------------
	// general variables
	int face_up = 0;
	int face_right = 32;
	int face_down = 64;
	int face_left = 96;

	// player's values
	float player_x;		// x-coordinate
	float player_y;		// y-coordinate
	float player_d;		// d = direction; sprite facing
	int player_h;		// health
	int player_spd;		// speed
	bool has_shield;	// checks if player has a shield
	bool weapon_1;		// checks if player has a weapon
	bool item_1;		// checks if player has an item; may end up getting replaced by more weapons instead

	// enemy values
	// note: may seem redundant here, but these may change in the future with custom names for each type of enemy
	float enemy_1x, enemy_1y;					// enemy 1 coords
	float enemy_2x, enemy_2y;					// enemy 2 coords
	float enemy_3x, enemy_3y;					// enemy 3 coords
	int enemy_1h, enemy_2h, enemy_3h;			// their health
	int enemy_1_spd, enemy_2_spd, enemy_3_spd;	// their speeds
	uint8_t enemy_1d, enemy_2d, enemy_3d;		// AI directions
	int view_1/*x, view_1y*/;
	//int view_2x, view_2y;
	//int view_3x, view_3y;			// view distance

	// initialize sprites --------------------------------------------------------------------
	olc::Decal* npc;
	olc::Decal* heart;
	olc::Decal* shield;

	// other variables
	enum direction { stop, up, right, down, left };		// directions match sprite facing in sprite sheet
	enum actions { attack, block, look_up, look_right, look_down, look_left };

	enum class state
	{
		MAIN_MENU,
		RUN_GAME
	};

	state gameState = state::RUN_GAME;

	// calculate the view radius from the enemy to chase after the player
	double viewRadius(double x1, double x2, double y1, double y2)
	{
		// x1,y1 belong to point 1
		// x2,y2 belong to point 2
		// sqrt = square root
		// pow = power to/of
		// 2 = ?? is this maybe the value of the power?
		return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	}

	bool OnUserCreate() override
	{
		// initialize variables...............................
		// player variables
		player_x = ScreenWidth() / 3 - 16.f;
		player_y = ScreenHeight() / 2 - 16.f;
		player_d = face_down;	// sprite facing
		player_h = 12;	// temp value for testing purposes
		player_spd = 70;
		has_shield = true;	// may set to false later for player to pick up an item

		// enemy variables
		enemy_1x = ScreenWidth() / 2 - 16.f;
		enemy_1y = ScreenHeight() / 2 - 16.f;
		enemy_1d = 0;	// sprite starting facing
		enemy_1h = 12;
		enemy_1_spd = 65;
		view_1 = 100;
		//enemy_1y = enemy_1y;
		
		//enemy_2x = ScreenWidth() / 3 - 16.f;
		//enemy_2y = ScreenHeight() / 2 - 16.f;
		//enemy_3x = ScreenWidth() / 3 * 2 - 16.f;
		//enemy_3y = ScreenHeight() / 2 - 16.f;
		//enemy_2d = 32;	// sprite starting facing
		//enemy_3d = 96;	// sprite starting facing
		//enemy_2h = 12;
		//enemy_3h = 12;
		//enemy_2_spd = 75;
		//enemy_3_spd = 85;
		//view_2 = 100;
		//view_3 = 100;

		// load sprites
		npc = new olc::Decal(new olc::Sprite("art/npc.png"));			// contains 6 rows of 4 facings
		heart = new olc::Decal(new olc::Sprite("art/health.png"));		// 5 pictures of hearts
		shield = new olc::Decal(new olc::Sprite("art/shield.png"));		// 4 images showing player is protected, based on facing

		return true;
	}

	void SetupPlayer()
	{
		// draw player
		// currently green triangle
		DrawPartialDecal({ player_x, player_y }, { 32,32 }, npc, { player_d,0 }, { 32,32 });

		// draw health bar
		DrawPartialDecal({ 5,5 }, { 32,32 }, heart, { 0,0 }, { 32,32 });
		DrawPartialDecal({ 42,5 }, { 32,32 }, heart, { 0,0 }, { 32,32 });
		DrawPartialDecal({ 79,5 }, { 32,32 }, heart, { 0,0 }, { 32,32 });
	}

	// following likely isn't needed as it could very easily be merged into the `SetupPlayer` function
	void SetupEnemy()
	{
		DrawPartialDecal({ enemy_1x, enemy_1y }, { 32,32 }, npc, { enemy_1d + 0.f,32 }, { 32,32 });	// red
		//DrawPartialDecal({ enemy_2x, enemy_2y }, { 32,32 }, npc, { enemy_2d + 0.f,64 }, { 32,32 });	// blue
		//DrawPartialDecal({ enemy_3x, enemy_3y }, { 32,32 }, npc, { enemy_3d + 0.f,96 }, { 32,32 });	// purple
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// draw canvas
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(96, 160, 96));

		// drawing border; probably won't be needed due to game design
		//for (int x = 0; x < ScreenWidth(); x++)
		//	for (int y = 0; y < 16; y++)
		//		Draw(x, y, olc::Pixel(0, 128, 0));

		// AI movement
		bool makesAMove = rand() % 100 >= 2;

		// Running the game...................................
		SetupEnemy();
		SetupPlayer();

		// moving player......................................
		{
			// move left
			if (GetKey(olc::Key::A).bHeld)
			{
				// wall collision
				if (player_x - player_spd * fElapsedTime >= 0)
				{
					player_x -= player_spd * fElapsedTime;
					player_d = 96;
				}
				else
				{
					player_x = 1;
				}
			}
			// move right
			if (GetKey(olc::Key::D).bHeld)
			{
				// wall collision
				if (player_x + player_spd * fElapsedTime <= ScreenWidth() - 31)
				{
					player_x += player_spd * fElapsedTime;
					player_d = 32;
				}
				else
				{
					player_x = ScreenWidth() - 32;
				}
			}
			// move up
			if (GetKey(olc::Key::W).bHeld)
			{
				// wall collision
				if (player_y - player_spd * fElapsedTime >= 0)
				{
					player_y -= player_spd * fElapsedTime;
					player_d = 0;
				}
				else
				{
					player_y = 1;
				}
			}
			// move down
			if (GetKey(olc::Key::S).bHeld)
			{
				// wall collision
				if (player_y + player_spd * fElapsedTime <= ScreenHeight() - 31)
				{
					player_y += player_spd * fElapsedTime;
					player_d = 64;
				}
				else
				{
					player_y = ScreenHeight() - 32;
				}
			}
			// speed boost for testing purposes
			if (GetKey(olc::Key::SHIFT).bHeld)
			{
				player_spd = 200;
			}
			else
			{
				player_spd = 70;
			}
		}

		// player facing test.................................
		{
			// may or may not keep these controls; they're neat, but primarily a test (and mostly successful imo)
			// look left
			if (GetKey(olc::Key::A).bPressed)
			{
				player_d = 96;
			}
			// look right
			if (GetKey(olc::Key::D).bPressed)
			{
				player_d = 32;
			}
			// look up
			if (GetKey(olc::Key::W).bPressed)
			{
				player_d = 0;
			}
			// look down
			if (GetKey(olc::Key::S).bPressed)
			{
				player_d = 64;

			}
		}


		// moving enemy 1.....................................
		
		//{
		//	// move left
		//	if (GetKey(olc::Key::LEFT).bHeld)
		//	{
		//		// wall collision
		//		if (enemy_1x - enemy_1_spd * fElapsedTime >= 0)
		//		{
		//			enemy_1x -= enemy_1_spd * fElapsedTime;
		//			enemy_1d = 96;
		//		}
		//		else
		//		{
		//			enemy_1x = 1;
		//		}
		//	}
		//	// move right
		//	if (GetKey(olc::Key::RIGHT).bHeld)
		//	{
		//		// wall collision
		//		if (enemy_1x + enemy_1_spd * fElapsedTime <= ScreenWidth() - 31)
		//		{
		//			enemy_1x += enemy_1_spd * fElapsedTime;
		//			enemy_1d = 32;
		//		}
		//		else
		//		{
		//			enemy_1x = ScreenWidth() - 32;
		//		}
		//	}
		//	// move up
		//	if (GetKey(olc::Key::UP).bHeld)
		//	{
		//		// wall collision
		//		if (enemy_1y - enemy_1_spd * fElapsedTime >= 0)
		//		{
		//			enemy_1y -= enemy_1_spd * fElapsedTime;
		//			enemy_1d = 0;
		//		}
		//		else
		//		{
		//			enemy_1y = 1;
		//		}
		//	}
		//	// move down
		//	if (GetKey(olc::Key::DOWN).bHeld)
		//	{
		//		// wall collision
		//		if (enemy_1y + enemy_1_spd * fElapsedTime <= ScreenHeight() - 31)
		//		{
		//			enemy_1y += enemy_1_spd * fElapsedTime;
		//			enemy_1d = 64;
		//		}
		//		else
		//		{
		//			enemy_1y = ScreenHeight() - 32;
		//		}
		//	}
		//}

		// AI Enemy 1.........................................
		
		//switch (enemy_1d)
		//{
		//case stop:
		//	//enemy_1_spd = 0;
		//	break;
		//case up:
		//	enemy_1y -= enemy_1_spd * fElapsedTime;
		//	break;
		//case right:
		//	enemy_1x += enemy_1_spd * fElapsedTime;
		//	break;
		//case down:
		//	enemy_1y += enemy_1_spd * fElapsedTime;
		//	break;
		//case left:
		//	enemy_1x -= enemy_1_spd * fElapsedTime;
		//	break;
		//}

		// enemy targets player test
		
		//if (player_x > enemy_1x)
		//{
		//	enemy_1x += enemy_1_spd * fElapsedTime;
		//}
		//if (player_x < enemy_1x)
		//{
		//	enemy_1x -= enemy_1_spd * fElapsedTime;
		//}
		//if (player_y > enemy_1y)
		//{
		//	enemy_1y += enemy_1_spd * fElapsedTime;
		//}
		//if (player_y < enemy_1y)
		//{
		//	enemy_1y -= enemy_1_spd * fElapsedTime;
		//}

		// enemy view test
		//if (player_x + view_1 > enemy_1x)
		//{
		//	enemy_1x -= enemy_1_spd * fElapsedTime;
		//}

		if (viewRadius(player_x, enemy_1x, player_y, enemy_1y) > (enemy_1x + view_1))
		{
			//enemy_1x -= enemy_1_spd * fElapsedTime;
			//left;
		}

		if (player_x < enemy_1x)
		{
			enemy_1x -= enemy_1_spd * fElapsedTime;
		}
		if (player_x > enemy_1x)
		{
			enemy_1x += enemy_1_spd * fElapsedTime;
		}
		if (player_y < enemy_1y)
		{
			enemy_1y -= enemy_1_spd * fElapsedTime;
		}
		if (player_y > enemy_1y)
		{
			enemy_1y += enemy_1_spd * fElapsedTime;
		}

		// temp code for testing purposes
		if (GetKey(olc::Key::SPACE).bPressed)
		{
			std::cout << "took damage" << std::endl;
			player_h--;
		}

		// determining game ending
		// player death
		if (player_h <= 0)
		{
			std::cout << "GAME OVER!" << std::endl;

			olc::vi2d textSize = GetTextSize("YOU DIED!") / 2;
			DrawStringDecal({ ScreenWidth() / 2 + 0.f, ScreenHeight() / 2 + 0.f }, "YOU DIED!", olc::RED, { (1.0f), (1.0f) });
			DrawPartialDecal({ player_x, player_y }, { 32, 32 }, npc, { player_d,32 }, { 32,32 });

			if (GetKey(olc::Key::ENTER).bPressed)
			{
				gameState = state::MAIN_MENU;
			}
		}

		// damage test
		// needs work due to enemies and player requiring a hit box
		//if (player_x == enemy_1x && player_y == enemy_1y)
		//{
		//	std::cout << "[Player] was hurt!!" << std::endl;
		//	player_h--;
		//}

		return true;
	}
};

int main()
{
	KniteGuard knite;
	if (knite.Construct(640, 360, 2, 2))
		knite.Start();

	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//		END OF FILE
//----------------------------------------------------------------------------------------------------------------------------------------