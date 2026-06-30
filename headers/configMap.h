#ifndef CONFIGMAP_H
#define CONFIGMAP_H

#define screenWidth 640
#define screenHeight 480

// vitesse
#define moveSpeed 0.5
#define rotSpeed 0.5

// spawn
#define beginPosX 12
#define beginPosY 2

// texture
#define texWidth 64
#define texHeight 64

// color printf
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define PURPLE "\033[0;35m"
#define RESET "\033[0m"

enum keys {
	AVANCER = 122,
	RECULER = 115,
	GAUCHE = 113,
	DROITE = 100
};

typedef struct Rays
{
	double		rayDirX;
	double		rayDirY;
}		rays;

typedef struct player
{
	double		posX, posY;
	double		dirX, dirY;
	double		planeX, planeY;
	int		mapX, mapY;
}		player;

typedef struct	s_data
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
	int		width;
	int		height;
}		t_data;

typedef struct textures
{
	t_data	floor;
	t_data	sky;
	t_data	wall;
	t_data	obstacle;
	t_data	gun;
}		tex;

typedef struct sprites
{
	float		x;
	float		y;
	int		width;
	int		height;
	int		uDiv;
	int		vDiv;
	int		vMove;
	struct sprites	*next;
	t_data	img;
}		sprites;

typedef struct s_game
{
	void	*mlx;
	void	*win;
	t_data	img;
	t_data	clearedBuf;
	rays	rays;
	player	player;
	tex textures;
	int		numSprites;
	sprites *sprites;
	double	Zbuffer[screenWidth];
}		t_game;

typedef struct vectors
{
	int x;
	int y;
}		vector;

#endif
