#if !defined(SEAMCARVING_H)
#define SEAMCARVING_H

#include <stdlib.h>
#include <stdint.h>
#include "c_img.h"

int d_y_2(struct rgb_img *im, int y, int x);
int d_x_2(struct rgb_img *im, int y, int x);
void calc_energy(struct rgb_img *im, struct rgb_img **grad);
void dynamic_seam(struct rgb_img *grad, double **best_arr);
void recover_path(double *best, int height, int width, int **path);
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path);

#endif