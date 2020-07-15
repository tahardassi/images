#include "ima.h"
#include <limits.h>
#include <string.h>



/*haut, haut-droite, droite, bas-droite, bas, bas-gauche, gauche, haut-gauche*/
static const int voisins[][2] = {{0, -1},{1, -1},{1, 0},{1, 1},{0, 1},{-1, 1},{-1, 0},{-1, -1}};
/*===============================================================*/
static inline int carre(int nb)
{
	return(nb * nb);
}

static int dEntre2Px(pixel_t px1, pixel_t px2)
{
	return(carre(px2.r - px1.r) + carre(px2.g - px1.g) + carre(px2.b - px1.b));
}

static int comp(pixel_t px1, pixel_t px2)
{
	return(px1.r > px2.r &&  px1.g > px2.g  &&  px1.b > px2.b);
}

static int compNotEgal(pixel_t px1, pixel_t px2)
{
	return(px1.r != px2.r && px1.g != px2.g && px1.b != px2.b);
}

int dAng(hsv_t px,hsv_t pv)
{
	int res;
	res = (int)(px.h - pv.h);
	if(res < 0)
	{
		if(res < -180)
			return(360 + res);
		return(- res);
	}
	else
	{
		if(res > 180)
			return(360 - res);
		return res;
	}
}

void teinteCompl(hsv_t * couleur, hsv_t * p)
{
	if(couleur->h >= 180)
		p->h = couleur->h - 180;
	else
		p->h = couleur->h + 180;
}

static void rgb2hsv(pixel_t * px, hsv_t * hsv)
{
	double min, max, delta, r, g, b;
	
	r = (double)px->r * 100. / UCHAR_MAX; g = (double)px->g * 100. / UCHAR_MAX; b = (double)px->b * 100. / UCHAR_MAX;
	min = Min(r, g);
	min = Min(min, b);
	max = Max(r, g);
	max = Max(max, b);
	hsv->v = max;
	delta = max - min;
	if(delta == 0 || max == 0)
	{
		hsv->h = 0;
		hsv->s = 0;
		return;
	}
	else
	{
		hsv->s = delta * 100. / max;
		if(max == r)
		{
			hsv->h = (int)(60. * (((g - b) / delta) + 360.)) % 360 ;
			return;
		}
		else if(max == g)
		{
			hsv->h = 60.0 * (((b - r) / delta) + 2.0);
			return;
		}
		else
		{
			hsv->h = 60. * (((r - g) / delta) + 4.);
			return;
		}
	}
}

static void hsv2rgb(hsv_t * px, pixel_t * rgb)
{
	double f, l, m, n, r,g,b,h,s,v;
	int i;
	s = px->s / 100.;
	v = px->v / 100.;
	h = px->h;
	h /= 60.;
	i = (int)h;
	f = h - i;
	l = v * (1. - s);
	m = v * (1. - (s * f));
	n = v * (1. - (s * (1. - f)));
	switch(i)
	{
		case 0:
			r = v;
			g = n;
			b = l;
			break;
		case 1:
			r = m;
			g = v;
			b = l;
			break;
		case 2:
			r = l;
			g = v;
			b = n;
			break;
		case 3:
			r = l;
			g = m;
			b = v;
			break;
		case 4:
			r = n;
			g = l;
			b = v;
			break;
		case 5:
		default:
			r = v;
			g = l;
			b = m;
			break;
	}
	r = r *255; g = g * 255; b = b * 255;
	rgb->r = (GLubyte)r; rgb->g = (GLubyte)g; rgb->b = (GLubyte)b;
}

void moyenneVoisinage(Image * image)
{
	unsigned long  i, j, x, y, voisin_x, voisin_y, X, Y, size;
	unsigned int r, g, b;

	GLubyte * im, *tmp;

	X = image->sizeX;
	Y = image->sizeY;
	size =  3 * X * Y;
	im = image->data;

	tmp = (GLubyte*)malloc((size_t) size * sizeof (GLubyte));
	assert(tmp);


	pixel_t * pim = (pixel_t*) im;
	pixel_t * ptmp = (pixel_t*) tmp;


	for (y = 1; y < Y - 1; ++y)
	{
		for(x = 1; x < X - 1; ++x)
		{
			i = y * X + x;

			r = pim[i].r; g = pim[i].g; b = pim[i].b;

			for(j = 0; j < NB_VOISINS; ++j)
			{
				voisin_x = x + voisins[j][0];
				voisin_y = y + voisins[j][1];

				r += pim[voisin_y * X + voisin_x].r;
				g += pim[voisin_y * X + voisin_x].g;
				b += pim[voisin_y * X + voisin_x].b;
			}
			r /= (NB_VOISINS + 1); g /= (NB_VOISINS + 1); b /= (NB_VOISINS + 1);
			ptmp[i].r = (GLubyte)r;
			ptmp[i].g = (GLubyte)g;
			ptmp[i].b = (GLubyte)b;
		}
	}
	image->data = tmp;
	free(im);
}


void moyennePondereeVoisinage(Image * image)
{
	unsigned long j , i, x, y, X, Y, size, voisin_x, voisin_y;
	unsigned int r, g, b, poid;
	GLubyte * im, *tmp;

	X = image->sizeX;
	Y = image->sizeY;
	size =  3 * Y * X;

	im = image->data;

	tmp = (GLubyte*)malloc((size_t) size * sizeof (GLubyte));
	assert(tmp);


	pixel_t * pim = (pixel_t*) im;
	pixel_t * ptmp = (pixel_t*) tmp;


	for (y = 1; y < Y - 1; ++y)
	{
		for(x = 1; x < X - 1; ++x)
		{
			i = y * X + x;

			r = pim[i].r; g = pim[i].g; b = pim[i].b;
			/*poid de 4 pour le pixel actuel*/
			r <<= 2; g <<= 2; b <<= 2;
			for(j = 0; j < NB_VOISINS; ++j)
			{
				poid = 1;/*poid de  1 pour les voisins h-d, b-d, b-g, h-g*/
				voisin_x = x + voisins[j][0];
				voisin_y = y + voisins[j][1];

				if(voisins[j][0] == 0 || voisins[j][1] == 0) poid = 2;/*poid de 2 pour les voisin h, d, b, g*/
				r += (pim[voisin_y * X + voisin_x].r * poid);
				g += (pim[voisin_y * X + voisin_x].g * poid);
				b += (pim[voisin_y * X + voisin_x].b * poid);
			}
			r >>= 4; g >>= 4; b >>= 4;
			ptmp[i].r = (GLubyte)r;
			ptmp[i].g = (GLubyte)g;
			ptmp[i].b = (GLubyte)b;
		}
	}
	image->data = tmp;
	free(im);
}



void contraste(Image * image)
{
	int j, i, x, y, X, Y, voisin_x, voisin_y, size, delta = 0, d, iv, r, g, b;

	GLubyte * im, * tmp;

	X = image->sizeX;
	Y = image->sizeY;
	size =  3 * X * Y;
	im = image->data;

	tmp = (GLubyte*)malloc((size_t) size * sizeof (GLubyte));
	assert(tmp);

	/*copie l'integralité de l'image*/
	memcpy(tmp, im, size);

	pixel_t * pim = (pixel_t*) im;
	pixel_t * ptmp = (pixel_t*) tmp;


	for (y= 1; y < Y - 1; ++y)
	{
		for(x = 1; x < X - 1; ++x)
		{
			i = y * X + x;

			for(j = 0, delta = 0; j < NB_VOISINS; ++j)
			{
				voisin_x = x + voisins[j][0];
				voisin_y = y + voisins[j][1];


				d = dEntre2Px(pim[i], pim[voisin_y * X + voisin_x]);
				delta = Max(delta, d);
				if(d == delta)
					iv = voisin_y * X + voisin_x;
				/*si un des voisins est déjà modifié suite à un contraste, passe au pixel suivant*/
				if(compNotEgal(pim[iv], ptmp[iv])) break;
			}
			r = pim[i].r; g = pim[i].g; b = pim[i].b;

			if(delta >= carre(SEUIL))/*  le seuil pour lequel on admet un contarste*/
			{
				if(comp(pim[i], pim[iv]))
				{
					/*augmente les composantes de la couleur à la position i*/
					if((r += (r/3)) > UCHAR_MAX)
						r = UCHAR_MAX;
					ptmp[i].r = (GLubyte)r;/*ecrase l'ancienne valeur*/

					if((g += (g/3)) > UCHAR_MAX)
						g = UCHAR_MAX;
					ptmp[i].g = (GLubyte)g;

					if((b += (b/3)) > UCHAR_MAX)
						b = UCHAR_MAX;
					ptmp[i].b = (GLubyte)b;
				}
				else
				{
					/*deminue les composantes de la couleur à la position i*/
					if((r -= (r/3)) < 0)
						r = 0;
					ptmp[i].r = (GLubyte)r;

					if((g -= (g/3)) < 0)
						g = 0;
					ptmp[i].g = (GLubyte)g;

					if((b -= (b/3)) < 0)
						b = 0;
					ptmp[i].b = (GLubyte)b;
				}
			}
		}
	}
	image->data = tmp;
	free(im);
}



void contraste_teinte(Image * image)
{
	unsigned long i, j, x, y, voisin_x, voisin_y, X, Y, size, iv;
	GLubyte * im, *tmp;
	int max, delta;
	hsv_t px, pv;
	/*h, h-d, d, b-d, b, b-g, g, h-g*/
	const int tab[][2] = {{0, -1},{1, -1},{1, 0},{1, 1},{0, 1},{-1, 1},{-1, 0},{-1, -1}};

	X = image->sizeX;
	Y = image->sizeY;
	size =  3 * X * Y;
	im = image->data;

	tmp = (GLubyte*)malloc((size_t) size * sizeof (GLubyte));
	assert(tmp);

	memcpy(tmp, im, size);

	pixel_t * pim = (pixel_t*) im;
	pixel_t * ptmp = (pixel_t*) tmp;


	for (y = 1; y < Y - 1; ++y)
	{
		for(x = 1; x < X - 1; ++x)
		{
			i = y * X + x;
			rgb2hsv(&pim[i], &px);
			max = 0;
			for(j = 0; j < NB_VOISINS; ++j)
			{
				delta = 0;
				voisin_x = x + tab[j][0];
				voisin_y = y + tab[j][1];
				rgb2hsv(&pim[voisin_y * X + voisin_x], &pv);
				delta = dAng(px, pv);
				max = Max(max, delta);
				if(max  == delta)
					iv = voisin_y*X+voisin_x;
			}
			rgb2hsv(&pim[iv], &pv);
			assert(max >= 0 && max <= 180);
			assert(px.h >= 0. && px.h <= 360.);
			if(max > 60)
			{
				teinteCompl(&pv, &px);
				hsv2rgb(&px, &ptmp[i]);
			}
		}
	}
	image->data = tmp;
	free(im);
}