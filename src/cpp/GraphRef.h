#include<string>
#include <type_traits>

#define GR_H_S 16
#define GR_H_M 32
#define GR_H_L 64

#define GR_W_S 32
#define GR_W_M 64
#define GR_W_L 128

#define RASTER_H 1000
#define RASTER_W 1000

typedef char (*RasterRef)[RASTER_H][RASTER_W];

typedef char (*GraphRefS)[GR_H_S][GR_W_S];
typedef char (*GraphRefM)[GR_H_M][GR_W_M];
typedef char (*GraphRefL)[GR_H_L][GR_W_L];

template <class T>
void updatePriv(T raster, T patch, int offsetX, int offsetY, int height, int width)
{
    for (int i = height - 1; i > -1; i--) // draws from top to bottom
    {
      int row = height - i + offsetY; // in raster
      for (int j = 0; j < width; j++)
      {
        int col = j + offsetX; // in raster
        if (row > RASTER_H || col > RASTER_W)
        {
          // do nothing
        }
        else if ((*raster)[row][col] != (*patch)[i][j])
        { // only engage in i/o if we have to
          (*raster)[row][col] = (*patch)[i][j];
          printf("\033[%d;%dH", row, col); // move to char location
          printf("%c", (*raster)[i][j]); // update char on screen
        }
      }
    }
};

template <typename T>
class GraphRef {
private:
    T raster; // ref to full memoery representation of full rendered screen
    T patch; // ref to new patch to draw to raster
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;

    GraphRef(T raster, T patch, int offsetX, int offsetY)
      : raster(raster)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height{5}
      , width{5}
      {};
    
    void update() {};
};

template <>
class GraphRef<GraphRefS> {
private:
    GraphRefS raster;
    GraphRefS patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;

    GraphRef(string name, GraphRefS raster, GraphRefS patch, int offsetX, int offsetY)
      : name(name)
      , raster(raster)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_S)
      , width(GR_W_S) 
      {};

    void update()
    {
      updatePriv<GraphRefS>(raster, patch, offsetX, offsetY, height, width);
    };
};

template <>
class GraphRef<GraphRefM> {
private:
    GraphRefM raster;
    GraphRefM patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefM raster, GraphRefM patch, int offsetX, int offsetY)
      : name(name)
      , raster(raster)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_M)
      , width(GR_W_M) 
      {};

    void update()
    {
      updatePriv<GraphRefM>(raster, patch, offsetX, offsetY, height, width);
    };
};

template <>
class GraphRef<GraphRefL> {
private:
    GraphRefL raster;
    GraphRefL patch;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefL raster, GraphRefL patch, int offsetX, int offsetY)
      : name(name)
      , raster(raster)
      , patch(patch)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_L)
      , width(GR_W_L) 
      {};

    void update()
    {
      updatePriv<GraphRefL>(raster, patch, offsetX, offsetY, height, width);
    };
};
