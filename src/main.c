#include <mlx.h>
#include <stdio.h>
#include "map.h"
#include "configMap.h"

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

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
	void	*mlx;
	t_data img;

	
	mlx = mlx_init();
	g.win = mlx_new_window(mlx, screenWidth, screenHeight, "Test");
	img.img = mlx_new_image(mlx, screenWidth, screenHeight);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
								&img.endian);

	my_mlx_pixel_put(&img, 5, 5, 0x00FF0000);
	mlx_put_image_to_window(mlx, g.win, img.img, 0, 0);
	mlx_loop(mlx);
}
