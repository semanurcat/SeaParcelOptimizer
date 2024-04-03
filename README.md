# SeaParcelOptimizer


## Usage

1. Run the compiled executable. The program will prompt you for input:
- The line number from the web source to draw.
- The cost of platforms.
- The cost of drilling (between 1 and 10).
2. The tool will then display two graphics windows:
- The first window shows the original polygon(s) based on the chosen line number.
- The second window shows the divided parcels within each polygon, optimized for cost and efficiency.

## Features

- **Data Extraction**: Utilizes CURL to download coordinates data from a specified URL.
- **Polygon Drawing**: Generates graphical representations of exploration areas based on input coordinates.
- **Area Calculation and Division**: Calculates the area of polygons and divides them into optimal parcels.
- **Cost-Profit Analysis**: Allows input of drilling and platform costs to calculate total costs and potential profits.
- **Graphical Visualization**: Uses SDL for rendering both the original and divided exploration areas.

## Dependencies

- [SDL2](https://www.libsdl.org/download-2.0.php) for graphical output.
- [CURL](https://curl.se/) for downloading data from web sources.
- A C Compiler (GCC, Clang, etc.).

## Configuration

No additional configuration is needed beyond ensuring that SDL2 and CURL are properly installed and accessible to the compiler.

## Examples

After running the program and providing the required inputs, two windows will appear:

1. The first window shows the undivided exploration area as defined by the input coordinates.
2. The second window displays the optimized division of the exploration area, with different colors representing various parcel sizes.

