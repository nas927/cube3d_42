#ifndef CONFIGMAP_H
#define CONFIGMAP_H

#define screenWidth 640
#define screenHeight 480

// vitesse
#define moveSpeed 0.05
#define rotSpeed 0.03

typedef struct s_game
{
	void	*mlx;
	void	*win;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}		t_game;

#endif
