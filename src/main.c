#include <mlx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"
#include "configMap.h"

void	my_mlx_put_pixel(t_data *data, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || x >= screenWidth || y < 0 || y >= screenHeight)
		return; // éviter d'écrire hors écran

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

void	clear_image(t_game *game)
{
	for (int y = 0; y < screenHeight; y++)
	{
		for(int x = 0; x < screenWidth; x++)
			my_mlx_put_pixel(&game->clearedBuf, x, y, 0x000000);
	}
	mlx_put_image_to_window(game->mlx, game->win, game->clearedBuf.img, 0, 0);
}

int		handleKeyBoard(int keycode, t_game *game)
{
	if (keycode == AVANCER)
	{
		if(worldMap[(int)game->player.posX + (int)game->player.dirX * (int)moveSpeed][(int)game->player.posY] == 0) 
			game->player.posX += game->player.dirX * moveSpeed;
		if(worldMap[(int)game->player.posX][(int)game->player.posY + (int)game->player.dirY * (int)moveSpeed] == 0)
			game->player.posY += game->player.dirY * moveSpeed;
		printf("J'avance !\n");	
	}
	else if (keycode == RECULER)
	{
		if(worldMap[(int)game->player.posX - (int)game->player.dirX * (int)moveSpeed][(int)game->player.posY] == 0) 
			game->player.posX -= game->player.dirX * moveSpeed;
		if(worldMap[(int)game->player.posX][(int)game->player.posY - (int)game->player.dirY * (int)moveSpeed] == 0)
			game->player.posY -= game->player.dirY * moveSpeed;
		printf("Je recule !\n");
	}
	else if (keycode == GAUCHE)
	{
		//both camera direction and camera plane must be rotated
		double oldDirX = game->player.dirX;
		game->player.dirX = game->player.dirX * cos(rotSpeed) - game->player.dirY * sin(rotSpeed);
		game->player.dirY = oldDirX * sin(rotSpeed) + game->player.dirY * cos(-rotSpeed);
		double oldPlaneX = game->player.planeX;
		game->player.planeX = game->player.planeX * cos(rotSpeed) - game->player.planeY * sin(rotSpeed);
		game->player.planeY = oldPlaneX * sin(rotSpeed) + game->player.planeY * cos(rotSpeed);
		printf("Je tourne à gauche !\n");
	}
	else if (keycode == DROITE)
	{
		//both camera direction and camera plane must be rotated
		double oldDirX = game->player.dirX;
		game->player.dirX = game->player.dirX * cos(-rotSpeed) - game->player.dirY * sin(-rotSpeed);
		game->player.dirY = oldDirX * sin(-rotSpeed) + game->player.dirY * cos(-rotSpeed);
		double oldPlaneX = game->player.planeX;
		game->player.planeX = game->player.planeX * cos(-rotSpeed) - game->player.planeY * sin(-rotSpeed);
		game->player.planeY = oldPlaneX * sin(-rotSpeed) + game->player.planeY * cos(-rotSpeed);
		printf("Je tourne à droite !\n");

	}
	else
		return 0;
	if (worldMap[(int)game->player.posX][(int)game->player.posY])
	{
		game->player.posX = beginPosX;
		game->player.posY = beginPosY; 
	}

	printf(YELLOW "Position du joueur X, Y : %f, %f\n" RESET, game->player.posX, game->player.posY);
	printf("Key %d est activé !\n", keycode);
	clear_image(game);
	return 0;
}

void	rotateTex(t_game *game, t_data *texture, float degres)
{
	int height = texture->height;
	int width = texture->width;
	double angle = degres * M_PI / 180.0;
	t_data newImage;
	newImage.img = mlx_new_image(game->mlx, width, height);
	newImage.addr = mlx_get_data_addr(newImage.img, &newImage.bits_per_pixel, &newImage.line_length, &newImage.endian);

	printf("gun angle : %f\n", angle);
	for (int y = 0 ; y < height ; y++)
		for (int x = 0 ; x < width ; x++)
		{
			double cx = x - (width >> 1);
			double cy = (height >> 1) - y;

			double rx = cx * cos(angle) - cy * sin(angle);
			double ry = cx * sin(angle) + cy * cos(angle);

			int rotX = rx + (width >> 1);
			int rotY = (height >> 1) - ry;
			char *imageOffset = newImage.addr + (y * newImage.line_length + x * (newImage.bits_per_pixel / 8));
			int oldOffset = (rotY * texture->line_length + rotX * (texture->bits_per_pixel / 8));
			int newColor = *(unsigned int*)(texture->addr + oldOffset);
			if (rotY >= height || rotX >= width || rotY < 0 || rotX < 0)
				newColor = 0xff000000;	
			*(unsigned int*)imageOffset = newColor;
		}

	mlx_destroy_image(game->mlx, texture->img);
	texture->img = newImage.img;
	texture->addr = newImage.addr;
	texture->line_length = newImage.line_length;
	texture->endian = newImage.endian;
	texture->height = height;
	texture->width = width;
}

int		load_textures_and_img(t_game *game)
{	
	t_data	*texs[5] = {&game->textures.wall, &game->textures.floor, &game->textures.sky, &game->textures.obstacle, &game->textures.gun};
	char	*texturePath[5] = {"eagle", "redbrick", "wood", "colorstone", "gun"};
	char	*folder = "pics/";
	char	*extension = ".xpm";
	char	finalPath[20] = "";
	for (int i = 0; i < 5; i++)
	{
		strcat(finalPath, folder);
		strcat(finalPath, texturePath[i]);
		strcat(finalPath, extension);
		strcat(finalPath, "\0");
		printf("texture init : %s\n", finalPath);

		texs[i]->width = 64;
		texs[i]->height = 64;
		if (i == 4)
		{
			texs[i]->height = 731;
			texs[i]->width = 566;
		}
		texs[i]->img = mlx_xpm_file_to_image(game->mlx, finalPath, &texs[i]->width, &texs[i]->height);
		texs[i]->addr = mlx_get_data_addr(texs[i]->img, &texs[i]->bits_per_pixel, &texs[i]->line_length, &texs[i]->endian);
		if (texs[i]->img == NULL)
			return 1;
		strcpy(finalPath,"");
	}
	rotateTex(game, texs[4], -4);
	game->clearedBuf.img = mlx_new_image(game->mlx, screenWidth, screenHeight);
	game->clearedBuf.addr = mlx_get_data_addr(game->clearedBuf.img, &game->clearedBuf.bits_per_pixel, &game->clearedBuf.line_length, &game->clearedBuf.endian);
	game->img.img = mlx_new_image(game->mlx, screenWidth, screenHeight);
	game->img.addr = mlx_get_data_addr(game->img.img, &game->img.bits_per_pixel, &game->img.line_length,&game->img.endian);
	return 0;
}

int		load_sprites(t_game *game)
{
	sprites *head = NULL;
	sprites *last = NULL;
	sprites *new;
	char	*texturePath[7] = {"pillar", "pillar", "pillar", "pillar", "pillar", "greenlight", "barrel"};
	char	*folder = "pics/";
	char	*extension = ".xpm";
	char	finalPath[20] = "";
	game->numSprites = 4;
	for (int i = 0; i < game->numSprites; i++)
	{
		strcat(finalPath, folder);
		strcat(finalPath, texturePath[i]);
		strcat(finalPath, extension);
		strcat(finalPath, "\0");
		printf("sprites init : %s\n", finalPath);
		new = malloc(sizeof(sprites));
		if (!new)
			return 1;
		int x = rand() % (mapWidth - 0 + 1) + 0;
		int y = rand() % (mapWidth - 0 + 1) + 0;
		*new = (sprites){x, y, texWidth, texHeight, 1, 1, 0, NULL, {NULL}};
		new->img.img = mlx_xpm_file_to_image(game->mlx, finalPath, &new->width, &new->height);
		new->img.addr = mlx_get_data_addr(new->img.img, &new->img.bits_per_pixel, &new->img.line_length, &new->img.endian);
		if (!head)
			head = new;
		else
			last->next = new;
		last = new;
		strcpy(finalPath, "");
	}
	game->sprites = head;
	return 0;
}

void	initTheGun(t_game *game)
{
	int beginTexX = 0;
	int beginTexY = (screenHeight >> 1);

	for (int y = beginTexY ; y < beginTexY + game->textures.gun.height ; y++)
		for (int x = beginTexX; x < beginTexX + game->textures.gun.width ; x++)
		{
			int offset = ((y - beginTexY) * game->textures.gun.line_length + x * (game->textures.gun.bits_per_pixel / 8));
			int color = *(unsigned int*)(game->textures.gun.addr + offset);
			// remove black arround texture
			if (color == (int)0xff000000)
				continue;
			my_mlx_put_pixel(&game->img, x, y, color);
		}
}

void	calculateHeightToDrawAndDraw(t_game *game, double perpWallDist, int side, int x)
{	
	//Calculate height of line to draw on screen
	int lineHeight = (int)(screenHeight / perpWallDist);
	//calculate lowest and highest pixel to fill in current stripe
	int drawStart = (-lineHeight >> 1) + (screenHeight >> 1);
	if(drawStart < 0) drawStart = 0;
	int drawEnd = (lineHeight >> 1) + (screenHeight >> 1);
	if(drawEnd >= screenHeight) drawEnd = screenHeight - 1;

	//calculate value of wallX
	double wallX; //where exactly the wall was hit
	if (side == 0) wallX = game->player.posY + perpWallDist * game->rays.rayDirY;
	else           wallX = game->player.posX + perpWallDist * game->rays.rayDirX;
	wallX -= floor(wallX);

	//x coordinate on the texture
	int texX = (int)(wallX * (double)texWidth);
	if(side == 0 && game->rays.rayDirX > 0) texX = texWidth - texX - 1;
	if(side == 1 && game->rays.rayDirY < 0) texX = texWidth - texX - 1;

	// How much to increase the texture coordinate per screen pixel
	double step = 1.0 * texHeight / lineHeight;
	// Starting texture coordinate
	double texPos = (drawStart - (screenHeight >> 1) + (lineHeight >> 1)) * step;
	for(int y = drawStart; y<drawEnd; y++)
	{
		// Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
		int texY = (int)texPos & (texHeight - 1);
		texPos += step;
		int offset = (texY * game->textures.wall.line_length + texX * (game->textures.wall.bits_per_pixel / 8));
		int color = *(unsigned int*)(game->textures.wall.addr + offset);
		//make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
		if(side == 1) color = (color >> 1) & 8355711;
		my_mlx_put_pixel(&game->img, x, y, color);
	}
}

void	drawFloorAndCeil(t_game *game)
{
	//FLOOR CASTING
	for(int y = 0; y < screenHeight; y++)
	{
		// rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
		float rayDirX0 = game->player.dirX - game->player.planeX;
		float rayDirY0 = game->player.dirY - game->player.planeY;
		float rayDirX1 = game->player.dirX + game->player.planeX;
		float rayDirY1 = game->player.dirY + game->player.planeY;

		// Current y position compared to the center of the screen (the horizon)
		int p = y - (screenHeight >> 1);

		// Vertical position of the camera.
		float posZ = 0.5 * screenHeight;

		// Horizontal distance from the camera to the floor for the current row.
		// 0.5 is the z position exactly in the middle between floor and ceiling.
		float rowDistance = posZ / p;

		// calculate the real world step vector we have to add for each x (parallel to camera plane)
		// adding step by step avoids multiplications with a weight in the inner loop
		float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
		float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;

		// real world coordinates of the leftmost column. This will be updated as we step to the right.
		float floorX = game->player.posX + rowDistance * rayDirX0;
		float floorY = game->player.posY + rowDistance * rayDirY0;

		for(int x = 0; x < screenWidth; ++x)
		{
			// the cell coord is simply got from the integer parts of floorX and floorY
			int cellX = (int)(floorX);
			int cellY = (int)(floorY);

			// get the texture coordinate from the fractional part
			int texX = (int)(texWidth * (floorX - cellX)) & (texWidth - 1);
			int texY = (int)(texHeight * (floorY - cellY)) & (texHeight - 1);

			floorX += floorStepX;
			floorY += floorStepY;

			int offset;
			int color;

			// floor
			offset = (texY * game->textures.floor.line_length + texX * (game->textures.floor.bits_per_pixel / 8));
			color = *(unsigned int*)(game->textures.floor.addr + offset);
			color = (color >> 1) & 8355711; // make a bit darker
			my_mlx_put_pixel(&game->img, x, y, color);

			//ceiling (symmetrical, at screenHeight - y - 1 instead of y)
			offset = (texY * game->textures.sky.line_length + texX * (game->textures.sky.bits_per_pixel / 8));
			color = *(unsigned int*)(game->textures.sky.addr + offset);
			my_mlx_put_pixel(&game->img, x, (screenHeight - y - 1), color);
		}
	}
}

void	drawSprites(t_game *game)
{
	sprites *spriteTex = game->sprites;
	//after sorting the sprites, do the projection and draw them
	for(int i = 0; i < game->numSprites; i++)
	{
		//translate sprite position to relative to camera
		double spriteX = spriteTex->x - game->player.posX;
		double spriteY = spriteTex->y - game->player.posY;

		//transform sprite with the inverse camera matrix
		// [ game->player.planeX   game->player.dirX ] -1                                       [ game->player.dirY      -game->player.dirX ]
		// [               ]       =  1/(game->player.planeX*game->player.dirY-game->player.dirX*game->player.planeY) *   [                 ]
		// [ game->player.planeY   game->player.dirY ]                                          [ -game->player.planeY  game->player.planeX ]

		double invDet = 1.0 / (game->player.planeX * game->player.dirY - game->player.dirX * game->player.planeY); //required for correct matrix multiplication

		double transformX = invDet * (game->player.dirY * spriteX - game->player.dirX * spriteY);
		double transformY = invDet * (-game->player.planeY * spriteX + game->player.planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D

		int spriteScreenX = (int)((screenWidth / 2) * (1 + transformX / transformY));
		int vMoveScreen = (int)(spriteTex->vMove / transformY);
		//calculate height of the sprite on screen
		int spriteHeight = abs((int)(screenHeight / (transformY))) / spriteTex->vDiv; //using 'transformY' instead of the real distance prevents fisheye
							       //calculate lowest and highest pixel to fill in current stripe
		int drawStartY = -spriteHeight / 2 + screenHeight / 2 + vMoveScreen;
		if(drawStartY < 0) drawStartY = 0;
		int drawEndY = spriteHeight / 2 + screenHeight/ 2 + vMoveScreen;
		if(drawEndY >= screenHeight) drawEndY = screenHeight - 1;

		//calculate width of the sprite
		int spriteWidth = abs( (int) (screenHeight / (transformY))) / spriteTex->uDiv;
		int drawStartX = -spriteWidth / 2 + spriteScreenX;
		if(drawStartX < 0) drawStartX = 0;
		int drawEndX = spriteWidth / 2 + spriteScreenX;
		if(drawEndX >= screenWidth) drawEndX = screenWidth - 1;

		//loop through every vertical stripe of the sprite on screen
		for(int stripe = drawStartX; stripe < drawEndX; stripe++)
		{
			int texX = (int)(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;
			//the conditions in the if are:
			//1) it's in front of camera plane so you don't see things behind you
			//2) it's on the screen (left)
			//3) it's on the screen (right)
			//4) ZBuffer, with perpendicular distance
			if(transformY > 0 && stripe > 0 && stripe < screenWidth && transformY < game->Zbuffer[stripe])
				for(int y = drawStartY; y < drawEndY; y++) //for every pixel of the current stripe
				{
					int d = (y - vMoveScreen) * 256 - screenHeight * 128 + spriteHeight * 128; //256 and 128 factors to avoid floats
					int texY = ((d * texHeight) / spriteHeight) / 256;
					char *gameColor = game->img.addr + (y * game->img.line_length + stripe * (game->img.bits_per_pixel / 8));
					char *color = spriteTex->img.addr + (texY * spriteTex->img.line_length + texX * (spriteTex->img.bits_per_pixel / 8));
					if((*(unsigned int*)color & 0x00FFFFFF) != 0)
					{
						unsigned int c = *(unsigned int*)color;
						c = (*(unsigned int*)gameColor >> 1) + (c >> 1);
						my_mlx_put_pixel(&game->img, stripe, y, c); //paint pixel if it isn't black, black is the invisible color
					}
				}
		}
		spriteTex = spriteTex->next; 
	}

}

void	drawWall(t_game *game)
{
	for(int x = 0; x < screenWidth; x++)
	{
		double cameraX = 2 * x / (double)screenWidth - 1; //x-coordinate in camera space
		game->rays.rayDirX = game->player.dirX + game->player.planeX * cameraX;
		game->rays.rayDirY = game->player.dirY + game->player.planeY * cameraX;
		//which box of the map we're in
		game->player.mapX = (int)game->player.posX;
		game->player.mapY = (int)game->player.posY;

		//length of ray from current position to next x or y-side
		double sideDistX;
		double sideDistY;

		//length of ray from one x or y-side to next x or y-side
		double deltaDistX = (game->rays.rayDirX == 0) ? 1e30 : fabs(1 / game->rays.rayDirX);
		double deltaDistY = (game->rays.rayDirY == 0) ? 1e30 : fabs(1 / game->rays.rayDirY);

		//what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;

		int hit = 0; //was there a wall hit?
		int side; //was a NS or a EW wall hit?
			  //calculate step and initial sideDist
		if (game->rays.rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (game->player.posX - game->player.mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (game->player.mapX + 1.0 - game->player.posX) * deltaDistX;
		}
		if (game->rays.rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (game->player.posY - game->player.mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (game->player.mapY + 1.0 - game->player.posY) * deltaDistY;
		}
		while (hit == 0)
		{
			//jump to next map square, either in x-direction, or in y-direction
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				game->player.mapX += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				game->player.mapY += stepY;
				side = 1;
			}
			//
			//ceiling (symmetrical, at screenHeight - y - 1 instead of y)
			//Check if ray has hit a wall
			if (worldMap[game->player.mapX][game->player.mapY] > 0) hit = 1;
		}
		double perpWallDist;
		if(side == 0) 	perpWallDist = (sideDistX - deltaDistX);
		else 		perpWallDist = (sideDistY - deltaDistY);
		game->Zbuffer[x] = perpWallDist; //perpendicular distance is used
		calculateHeightToDrawAndDraw(game, perpWallDist, side, x);
		mlx_put_image_to_window(game->mlx, game->win, game->img.img, 0, 0);
	}
}

int		render_next_frame(t_game *game) 
{
	drawFloorAndCeil(game);	
	drawWall(game);
	drawSprites(game);
	initTheGun(game);
	return 0;
}

int		main(void)
{
	t_game g;

	g.player.posX = beginPosX;
	g.player.posY = beginPosY;
	g.player.dirX = -1;
	g.player.dirY = 0;
	g.player.planeX = 0;
	g.player.planeY = 0.66;
	g.mlx = mlx_init();
	if (g.mlx == NULL)
		return 1;
	g.win = mlx_new_window(g.mlx, screenWidth, screenHeight, "Nouveau DOOM");
	if (g.win == NULL)
	{
		free(g.win);
		return 1;
	}
	if (load_textures_and_img(&g) == 1 || load_sprites(&g))
		return 1;
	mlx_key_hook(g.win, handleKeyBoard, &g);
	  mlx_loop_hook(g.mlx, render_next_frame, &g);
	  mlx_loop(g.mlx);
	  mlx_destroy_display(g.mlx);
	  free(g.mlx);
	  printf("end :(");
}
