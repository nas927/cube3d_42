#include <mlx.h>
#include <stdio.h>
#include "map.h"
#include "configMap.h"

int		render_next_frame(t_game g) 
{
	int		x;

	/*x = 0;
	while(x < screenWidth)
	{
		double cameraX = 2 * x / double(screenWidth) - 1; //x-coordinate in camera space
      		double rayDirX = dirX + planeX * cameraX;
     		double rayDirY = dirY + planeY * cameraX;


		x++;
	}*/
}

int	main(void)
{
	t_game g;

	g.mlx = mlx_init();
	g.win = mlx_new_window(g.mlx, screenWidth, screenHeight, "Raycaster");
	g.addr = mlx_get_data_addr(g.win, &g.bits_per_pixel, &g.line_length,
								&g.endian);

	mlx_loop_hook(g.mlx, render_next_frame, &g);
	// Un pixel bleu à la position (100, 100)
	mlx_loop(g.mlx);
}
