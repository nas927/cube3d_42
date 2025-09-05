#include "map.h"
#include "configMap.h"
#include "configEvent.h"

// gestion clavier
int	key_press(int keycode, t_game *g)
{
	if (keycode == 65307) // ESC
		exit(0);
	if (keycode == 'z') // avancer
	{
		if (worldMap[(int)(g->posX + g->dirX * moveSpeed)][(int)g->posY] == 0)
			g->posX += g->dirX * moveSpeed;
		if (worldMap[(int)g->posX][(int)(g->posY + g->dirY * moveSpeed)] == 0)
			g->posY += g->dirY * moveSpeed;
	}
	if (keycode == 's') // reculer
	{
		if (worldMap[(int)(g->posX - g->dirX * moveSpeed)][(int)g->posY] == 0)
			g->posX -= g->dirX * moveSpeed;
		if (worldMap[(int)g->posX][(int)(g->posY - g->dirY * moveSpeed)] == 0)
			g->posY -= g->dirY * moveSpeed;
	}
	if (keycode == 'd') // tourner droite
	{
		double oldDirX = g->dirX;
		g->dirX = g->dirX * cos(-rotSpeed) - g->dirY * sin(-rotSpeed);
		g->dirY = oldDirX * sin(-rotSpeed) + g->dirY * cos(-rotSpeed);
		double oldPlaneX = g->planeX;
		g->planeX = g->planeX * cos(-rotSpeed) - g->planeY * sin(-rotSpeed);
		g->planeY = oldPlaneX * sin(-rotSpeed) + g->planeY * cos(-rotSpeed);
	}
	if (keycode == 'q') // tourner gauche
	{
		double oldDirX = g->dirX;
		g->dirX = g->dirX * cos(rotSpeed) - g->dirY * sin(rotSpeed);
		g->dirY = oldDirX * sin(rotSpeed) + g->dirY * cos(rotSpeed);
		double oldPlaneX = g->planeX;
		g->planeX = g->planeX * cos(rotSpeed) - g->planeY * sin(rotSpeed);
		g->planeY = oldPlaneX * sin(rotSpeed) + g->planeY * cos(rotSpeed);
	}
	return (0);
}
