#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "seamcarving.h"


/////////////////////////////////////////////////////////////////////

// split into edge(need wrap-a-round) and normal cases:
// x== 0
// x == im ->height-1
// Calculate the delta x and the delta ys. They all need R,G,B!

int d_y_2(struct rgb_img *im, int y, int x) { // THIS FUNCTION WORKS

    // consideing the wrap-around. use modulo
    int y_prev = (y - 1 + im->height) % im->height;
    int y_next = (y + 1) % im->height;

    // simply following the formula that was given
    int Ry = get_pixel(im, y_next, x, 0) - get_pixel(im, y_prev, x, 0);
    int Gy = get_pixel(im, y_next, x, 1) - get_pixel(im, y_prev, x, 1);
    int By = get_pixel(im, y_next, x, 2) - get_pixel(im, y_prev, x, 2);

    return (Ry*Ry) + (Gy*Gy) + (By*By);
}


int d_x_2(struct rgb_img *im, int y, int x){ // THIS FUNCTION WORKS
    int x_prev = (x - 1 + im->width) % im->width;
    int x_next = (x + 1) % im->width;
    
    int Rx = get_pixel(im, y, x_next, 0) - get_pixel(im, y, x_prev, 0);
    int Gx = get_pixel(im, y, x_next, 1) - get_pixel(im, y, x_prev, 1);
    int Bx = get_pixel(im, y, x_next, 2) - get_pixel(im, y, x_prev, 2);

    return (Rx*Rx) + (Gx*Gx) + (Bx*Bx);
}

// Calculate the energy of pixels of the image!
void calc_energy(struct rgb_img *im, struct rgb_img **grad){ // THIS FUNCTION WORKS
   
   create_img(grad, im->height, im->width);
    
    int y = 0;
    int x = 0;

    while (y < im->height){
        x = 0;
        while (x < im->width){
            int dx = d_x_2(im, y, x);
            int dy = d_y_2(im, y, x);

            double energy = ((double)sqrt(dx + (double)dy))/10; // need divide by 10??? the nconvert to u8
            
            if (energy >255){
                energy = (double)255;
            }

            int converted_energy = (uint8_t)(energy);
            set_pixel(*grad, y, x, converted_energy, converted_energy, converted_energy);

            x++;
        }
        y++;
    }

    

}

// Plans for the function:  y-coord, x-coord
// directly above: (y-1, x)
// above-left: (y-1, x-1), x>0
// right     : (y-1, x+1), x < width - 1
// need to consider edge
void dynamic_seam(struct rgb_img *grad, double **best_arr){ // THIS FUNCTION WORKS

    *best_arr = (double *)malloc(sizeof(double)*(grad->height) * (grad->width));

    // FIRST ROW OF ARRAY
    for (int y = 0; y < grad->height; y++) {
        for (int x = 0; x < grad->width; x++){
            // first ROW
            if (y == 0){
                (*best_arr)[x] = get_pixel(grad, 0, x, 0);  
            }

            // OTHER ROW
            else{
                // this is actually equivalent to 2 coordinate [y][x]... 
                double min_energy = (*best_arr)[(y - 1) * grad->width + x];  // directly Above

                if (x > 0){
                    // check above-left:
                    min_energy = fmin(min_energy, (*best_arr)[(y - 1) * grad->width + (x - 1)]);    
                }

                if (x < grad->width - 1){
                     // check above-right:
                     min_energy = fmin(min_energy, (*best_arr)[(y - 1) * grad->width + (x + 1)]);
                }

                // current picel energy + the min energy from the 2 or 3 above choices
                (*best_arr)[(y * grad->width) + x] = get_pixel(grad, y, x, 0) + min_energy;
            }
        }
             
    }

}

// go backwards somehowwwww
void recover_path(double *best, int height, int width, int **path){
    
    *path = (int *)malloc(sizeof(int) * height); // create space for path
    
    // Find the smallest # in the last row to trace back up
    int min_column = 0;

    double min_value = best[(height - 1) * width + 0]; // 0th index of last row

    for (int x = 1; x < width; x++){

        double test_min = best[(height - 1) * width + x];


        if (test_min < min_value){
            min_value = test_min; // find mininum value
            min_column = x; // keep track of which column you are in!
        }
    }

    // put the smallest number (from the bottom row)in the path array
    (*path)[height - 1] = min_column;



    // trace backwards
    for (int y = height - 2; y >= 0; y--){
       
        // cur_x is the underneath-column
        int cur_x = (*path)[y+1]; // below-column/////////// FORGOT to uPDATE THHISSSS VERY IMRPOTANTTTTTT!!!!
        int opt_x = cur_x; // optimal path column initialization
        double best_val = best[y * width + cur_x]; // the cur pixel


        // check left pixel
        if (cur_x > 0 && best[y * width + cur_x - 1] < best_val) {
            opt_x = cur_x - 1;
            best_val = best[y * width + opt_x];
        }

        // check below-right pix (but this time it comapre to either the pixel that is above or to the left...depending on which was smaller)
        if (cur_x < width - 1 && best[y * width + cur_x + 1] < best_val) {
            opt_x = cur_x + 1;
        }

        (*path)[y] = opt_x;  // store best column in the path! done!

    }

}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    
    create_img(dest, src->height, (src->width)-1);


    for (int y = 0; y < src->height; y++){ // use destination or source>>>>>
        
        int dest_x = 0;  // Index for destination image////////// OMG I FORGOT THISSSSS VERY IMPORTANT!!!!!!!!!!
         
        for (int x = 0; x < src->width; x++){
            if (x == path[y]){
                continue; // don't do anything with the path that must be removed
            }
            else{

                // copy pixel from source to destination if it is NOT along the pathway
                set_pixel(*dest, y, dest_x, get_pixel(src, y, x, 0), get_pixel(src, y, x, 1), get_pixel(src, y, x, 2));
                
                dest_x ++;
            }
        }
    }


}

