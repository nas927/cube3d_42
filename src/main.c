#include <mlx.h>
#include <stdio.h>
#include "map.h"
#include "configMap.h"
#include "configEvent.h"

int	render(t_game *g)
{
	for (int x = 0; x < screenWidth; x++)
	{
		double cameraX = 2 * x / (double)screenWidth - 1;
		double rayDirX = g->dirX + g->planeX * cameraX;
		double rayDirY = g->dirY + g->planeY * cameraX;

		int mapX = (int)g->posX;
		int mapY = (int)g->posY;

		double sideDistX;
		double sideDistY;

		double deltaDistX = fabs(1 / rayDirX);
		double deltaDistY = fabs(1 / rayDirY);
		double perpWallDist;

		int stepX, stepY;
		int hit = 0;
		int side;

		if (rayDirX < 0) { stepX = -1; sideDistX = (g->posX - mapX) * deltaDistX; }
		else { stepX = 1; sideDistX = (mapX + 1.0 - g->posX) * deltaDistX; }
		if (rayDirY < 0) { stepY = -1; sideDistY = (g->posY - mapY) * deltaDistY; }
		else { stepY = 1; sideDistY = (mapY + 1.0 - g->posY) * deltaDistY; }

		while (hit == 0)
		{
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}
			if (worldMap[mapX][mapY] > 0) hit = 1;
		}

		if (side == 0) perpWallDist = (mapX - g->posX + (1 - stepX) / 2) / rayDirX;
		else           perpWallDist = (mapY - g->posY + (1 - stepY) / 2) / rayDirY;

		int lineHeight = (int)(screenHeight / perpWallDist);

		int drawStart = -lineHeight / 2 + screenHeight / 2;
		if (drawStart < 0) drawStart = 0;
		int drawEnd = lineHeight / 2 + screenHeight / 2;
		if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

		int color;
		if (worldMap[mapX][mapY] == 1) color = 0xFF0000;
		else if (worldMap[mapX][mapY] == 2) color = 0x00FF00;
		else if (worldMap[mapX][mapY] == 3) color = 0x0000FF;
		else if (worldMap[mapX][mapY] == 4) color = 0xFFFFFF;
		else color = 0xFFFF00;

		if (side == 1) color = color / 2;

		for (int y = drawStart; y < drawEnd; y++)
			mlx_pixel_put(g->mlx, g->win, x, y, color);
	}
	return (0);
}

int	main(void)
{
	t_game g;

	g.mlx = mlx_init();
	g.win = mlx_new_window(g.mlx, screenWidth, screenHeight, "Raycaster");
	g.posX = 22; g.posY = 12;
	g.dirX = -1; g.dirY = 0;
	g.planeX = 0; g.planeY = 0.66;

	mlx_loop_hook(g.mlx, render, &g);
	mlx_hook(g.win, 2, 1L<<0, key_press, &g); // key press
	mlx_loop(g.mlx);
}
