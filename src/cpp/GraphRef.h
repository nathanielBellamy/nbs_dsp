#include<string>
#include <type_traits>

#define GR_H_S 16
#define GR_H_M 32
#define GR_H_L 64

#define GR_W_S 32
#define GR_W_M 64
#define GR_W_L 128

#define RASTER_H 156
#define RASTER_W 156

typedef char (*RasterRef)[RASTER_H][RASTER_W];

typedef char (*GraphRefS)[GR_H_S][GR_W_S];
typedef char (*GraphRefM)[GR_H_M][GR_W_M];
typedef char (*GraphRefL)[GR_H_L][GR_W_L];

template <class T>
void updatePriv(RasterRef raster, T patch, int offsetX, int offsetY, int height, int width)
{
    for (int i = height - 1; i > -1; i--) // draws from top to bottom
    {
      int row = i + offsetY; // in raster
      for (int j = 0; j < width; j++)
      {
        int col = j + offsetX; // in raster
        char patchChar = (*patch)[i][j];
        if (row > RASTER_H || col > RASTER_W || patchChar == '\0')
        {
          // do nothing
        }
        else if ((*raster)[row][col] != patchChar)
        { // only engage in i/o if we have to
          (*raster)[row][col] = patchChar;
          if (patchChar == '#')
          {
            // Set color to cyan
            printf("\e[48;5;12m");
          }
          else if (patchChar == 'o')
          {
            // Set color to light blue
            printf("\e[48;5;25m");
          }
          else
          {
            // Set color to dark blue
            printf("\e[48;5;18m");
          }
          printf("\033[%d;%dH", row, col); // move to char location
          printf("%c", ' '); // update char on screen
          if (patchChar == '#')
          {
            // Reset text color to default
            printf("\033[0m");
          }
        }
      }
    }
};

template <typename T>
class GraphRef {
private:
    T patch; // ref to new patch to draw to raster
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;

    GraphRef(string name, T patch, int offsetX, int offsetY)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height{5}
      , width{5}
      {};
    
    void update(RasterRef raster) {};
};

template <>
class GraphRef<GraphRefS> {
private:
    GraphRefS patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;

    GraphRef(string name, GraphRefS patch, int offsetX, int offsetY)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_S)
      , width(GR_W_S) 
      {};

    void update(RasterRef raster)
    {
      updatePriv<GraphRefS>(raster, patch, offsetX, offsetY, height, width);
    };
};

template <>
class GraphRef<GraphRefM> {
private:
    GraphRefM patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefM patch, int offsetX, int offsetY)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_M)
      , width(GR_W_M) 
      {};

    void update(RasterRef raster)
    {
      updatePriv<GraphRefM>(raster, patch, offsetX, offsetY, height, width);
    };
};

template <>
class GraphRef<GraphRefL> {
private:
    GraphRefL patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefL patch, int offsetX, int offsetY)
      : name(name)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_L)
      , width(GR_W_L) 
      {};

    void update(RasterRef raster)
    {
      updatePriv<GraphRefL>(raster, patch, offsetX, offsetY, height, width);
    };
};
