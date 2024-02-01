#include <math.h>

#include "allocate.h"
#include "randlib.h"
#include "tiff.h"
#include "typeutil.h"

typedef struct pixel {
  int row, col;
} pixel_t;

void print_usage(const char *program_name);
void ConnectedNeighbors(pixel_t s, double T, unsigned char **img, int width,
                        int height, int *M, pixel_t c[4]);
void ConnectedSet(pixel_t s, double T, unsigned char **img, int width,
                  int height, int ClassLabel, unsigned int **seg,
                  int *NumConPixels);

/**
 * @brief Finds the connected neighbors of a pixel
 *
 * @param s the input pixel
 * @param T the threshold used for finding neighbors
 * @param img a 2D array of pixels
 * @param width the width of the image
 * @param height the height of the image
 * @param M a pointer to the number of neighbors connected to the pixels
 * @param c an array containing the M connected neighbors to pixel s, where M <=
 * 4
 *
 * Algorithm:
 * 1. iterate over neighbors in the image
 * 2. if neighbor is within threshold, add it to list of connected neighbors and
 * increment number of neighbors
 */
void ConnectedNeighbors(pixel_t s, double T, unsigned char **img, int width,
                        int height, int *M, pixel_t c[4]) {
  // Define directions for neighbors: up, down, left, right
  int dx[] = {0, 0, -1, 1};
  int dy[] = {-1, 1, 0, 0};

  *M = 0;  // Initialize the number of connected neighbors

  // Iterate over possible neighbors
  for (int i = 0; i < 4; i++) {
    int n_col = s.col + dx[i];  // col coordinate of the neighbor
    int n_row = s.row + dy[i];  // row coordinate of the neighbor

    // Check if the neighbor is within the boundaries of the image
    if (n_col >= 0 && n_col < width && n_row >= 0 && n_row < height) {
      // Check if the difference in intensity is within the threshold
      if (abs(img[s.row][s.col] - img[n_row][n_col]) <= T) {
        // Add the connected neighbor to the list
        c[*M].col = n_col;
        c[*M].row = n_row;
        (*M)++;  // Increment the count of connected neighbors
      }
    }
  }
}

/**
 * @brief
 *
 * @param s
 * @param T
 * @param img
 * @param width
 * @param height
 * @param ClassLabel
 * @param seg
 * @param NumConPixels
 */
void ConnectedSet(pixel_t s, double T, unsigned char **img, int width,
                  int height, int ClassLabel, unsigned int **seg,
                  int *NumConPixels) {
  // add seed pixel to queue
  pixel_t B[width * height];
  int B_idx = 0;
  B[B_idx] = s;
  do {
    // pop a pixel, set it in the output image, and increment connected count
    pixel_t s = B[B_idx--];
    seg[s.row][s.col] = ClassLabel;
    *NumConPixels += 1;
    // get connected neighbors for the popped pixel
    pixel_t neighbors[4];
    int num_neighbors = 0;
    ConnectedNeighbors(s, T, img, width, height, &num_neighbors, neighbors);
    // printf("num_neighbors: %d\t", num_neighbors);
    // add neighbors to the queue if not already a part of seg
    for (int i = 0; i < num_neighbors; i++) {
      // printf("neighbor: %d, %d\t", neighbors[i].col, neighbors[i].row);
      if (seg[neighbors[i].row][neighbors[i].col] != ClassLabel) {
        // printf("adding neighbor: %d, %d\t", neighbors[i].col,
        // neighbors[i].row);
        B[++B_idx] = neighbors[i];
      }
    }
    // printf("\n");
  } while (B_idx > 0);
}

int main(int argc, char **argv) {
  FILE *fp;
  struct TIFF_img input_img;

  if (argc != 3) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  double threshold = atof(argv[2]);

  // open image file
  if ((fp = fopen(argv[1], "rb")) == NULL) {
    fprintf(stderr, "Error: failed to open file %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  // read image
  if (read_TIFF(fp, &input_img)) {
    fprintf(stderr, "Error: failed to read file %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  // close image file
  fclose(fp);

  // check image data type
  if (input_img.TIFF_type != 'g') {
    fprintf(stderr, "Error: image must be 8-bit grayscale\n");
    return EXIT_FAILURE;
  }

  // create output seg image
  struct TIFF_img seg_img;
  get_TIFF(&seg_img, input_img.height, input_img.width, 'g');

  pixel_t s = {.col = 67, .row = 45};
  int connected_pixels = 0;
  ConnectedSet(s, 2, input_img.mono, input_img.width, input_img.height, 255,
               seg_img.mono, &connected_pixels);

  // open seg image file
  // Convert double to string
  char num_str[20];
  snprintf(num_str, sizeof(num_str), "%.2f", threshold);  // Example format %.2f

  // Construct file name with the double value
  char output_file[50];
  strcpy(output_file, "../img/seg_");
  strcat(output_file, num_str);
  strcat(output_file, ".tif");
  if ((fp = fopen(output_file, "wb")) == NULL) {
    fprintf(stderr, "Error: failed to open output file\n");
    return EXIT_FAILURE;
  }

  // write seg image
  if (write_TIFF(fp, &seg_img)) {
    fprintf(stderr, "Error: failed to write TIFF file %s\n", argv[2]);
    return EXIT_FAILURE;
  }

  // close seg image file
  fclose(fp);

  return EXIT_SUCCESS;
}

void print_usage(const char *program_name) {
  printf("Usage: %s <image-file-path>\n", program_name);
  printf("Arguments:\n");
  printf("  <image-file-path> : Specify the file path of the image.\n");
  printf(
      "  <threshold> : Specify the threshold number for determining pixel "
      "neighbors.\n");
}
