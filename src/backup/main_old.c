#include <mlx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
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
			my_mlx_put_pixel(&game->img, x, y, 0x000000);
	}
}

void	calculateHeightToDrawAndDraw(t_game *game, double perpWallDist, int side, int x)
{	
	//Calculate height of line to draw on screen
	int lineHeight = (int)(screenHeight / perpWallDist);
	//calculate lowest and highest pixel to fill in current stripe
	int drawStart = -lineHeight / 2 + screenHeight / 2;
	if(drawStart < 0) drawStart = 0;
	int drawEnd = lineHeight / 2 + screenHeight / 2;
	if(drawEnd >= screenHeight) drawEnd = screenHeight - 1;
	int texNum = worldMap[game->player.mapX][game->player.mapY] - 1;

	//Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
	double wallX; //where exactly the wall was hit
	if (side == 0) wallX = game->player.posY + perpWallDist * game->rays.rayDirY;
	else           wallX = game->player.posX + perpWallDist * game->rays.rayDirX;
	wallX -= floor((wallX));

	int texX = (int)wallX * (double)texWidth;
	if(side == 0 && game->rays.rayDirX > 0) texX = texWidth - texX - 1;
	if(side == 1 && game->rays.rayDirY < 0) texX = texWidth - texX - 1;
	// How much to increase the texture coordinate per screen pixel
	double step = 1.0 * texHeight / lineHeight;
	// Starting texture coordinate
	double texPos = (drawStart - screenHeight / 2 + lineHeight / 2) * step;
	int offset;
	int color;
	for(int y = drawStart; y<drawEnd; y++)
	{
		// Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
		int d = (drawStart * 256) - (screenHeight * 128) + lineHeight * 128;
		int texY = ((d * texHeight) / lineHeight) / 256;
		if (texX < 0 || texX >= texWidth || texY < 0
		|| texY >= texHeight)
			continue;
		texPos += step;
		offset = (texY * game->texture.img.line_length + texX * (game->texture.img.bits_per_pixel / 8));
		if (offset < 0 || offset + 3 >= texHeight
		* game->texture.img.line_length)
			continue;
		//make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
		color = *(int *)(game->texture.img.addr + offset);
		if(side == 1) color = (color >> 1) & 8355711;
		my_mlx_put_pixel(&game->img, x, y, color);
	}
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

	clear_image(game);
	printf(YELLOW "Position du joueur X, Y : %f, %f\n" RESET, game->player.posX, game->player.posY);
	printf("Key %d est activé !\n", keycode);
	return 0;
}

int		render_next_frame(t_game *game) 
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
			//Check if ray has hit a wall
			if (worldMap[game->player.mapX][game->player.mapY] > 0) hit = 1;
		}
		double perpWallDist;
		if(side == 0) perpWallDist = (sideDistX - deltaDistX);
      		else perpWallDist = (sideDistY - deltaDistY);
		calculateHeightToDrawAndDraw(game, perpWallDist, side, x);
		mlx_put_image_to_window(game->mlx, game->win, game->img.img, 0, 0);
	}
	return 0;
}

int		main(void)
{
	t_game g;

	g.texture.width = texWidth;
	g.texture.height = texHeight;
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
	g.texture.img.img = mlx_xpm_file_to_image(g.mlx, "pics/colorstone.xpm", &g.texture.width, &g.texture.height);
	if (g.texture.img.img == NULL)
		return 1;
	g.img.img = mlx_new_image(g.mlx, screenWidth, screenHeight);
	g.texture.img.addr = mlx_get_data_addr(g.texture.img.img, &g.texture.img.bits_per_pixel, &g.texture.img.line_length,&g.texture.img.endian);
	g.img.addr = mlx_get_data_addr(g.img.img, &g.img.bits_per_pixel, &g.img.line_length,&g.img.endian);
	mlx_key_hook(g.win, handleKeyBoard, &g);
	mlx_loop(g.mlx);
	mlx_loop_hook(g.mlx, render_next_frame, &g);
	mlx_destroy_image(g.mlx, g.img.img);
	mlx_destroy_image(g.mlx, g.texture.img.img);
	mlx_destroy_display(g.mlx);
	free(g.mlx);
}
