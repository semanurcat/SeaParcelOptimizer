#define SDL_MAIN_HANDLED
#include <curl.h>
#include <stdlib.h>
#include <SDL.h>
#include <cmath>
#include <stdio.h>
#include <algorithm>
#include <vector>
#define MIN_KARE_BOYUTU 1
#define MAX_KARE_BOYUTU 160
#define MAX 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRID_SIZE 10
#define MAX_KOSE_SAYISI 100 // Maksimum köşe sayısı
#define MAX_POINTS 10
char satir_no;
int i, j = 0;
int x[10], y[10], shape1_x[10], shape1_y[10], shape2_x[10], shape2_y[10];
int x_index = 0, y_index = 0;
char satir[100]; // Her satırın en fazla 100 karakter olduğunu varsayalım
int uzunluk;
int kose_sayisi;
int sayac = 0;
int matrix_boyutu = 0;

void txt(char satir_no) {
    const char* veri_indir = "curl http://abilgisayar.kocaeli.edu.tr/prolab1/prolab1.txt -o \"C:\\Users\\sema\\source\\repos\\proje\\data.txt\"";
    system(veri_indir);
    FILE* dosya;
    fopen_s(&dosya, "C:\\Users\\sema\\source\\repos\\proje\\data.txt", "r");
    while (fgets(satir, 100, dosya) != NULL) {
        if (satir[0] == satir_no) {
            printf_s("%s", satir);
            break;
        }
        else {
            continue;
        }
    }
    uzunluk = strlen(satir);

    for (i = 0; i < uzunluk; i++) {
        if (satir[i] == '(') {
            i++;
            int deger_x, deger_y;
            if (sscanf_s(satir + i, "%d,%d", &deger_x, &deger_y) == 2) {
                x[x_index] = deger_x;
                y[y_index] = deger_y;
                x_index++;
                y_index++;
                while (satir[i] != ')') {
                    i++;
                }
            }
        }
    }

    for (int i = 0; i < x_index; i++) {
        printf("x[%d]: %d, y[%d]: %d\n", i, x[i], i, y[i]);
    }

    fclose(dosya);
}


void split_polygons(int* x, int* y, int shapes[3][2][MAX_POINTS], int num_points[3]) {
    int i = 0;
    int sayac = 0;
    int s = 0; // Start with the first shape

    while (s < 3 && sayac < MAX_POINTS) { // Loop over each shape
        shapes[s][0][i] = x[sayac];
        shapes[s][1][i] = y[sayac];

        if (i > 0 && x[sayac] == shapes[s][0][0] && y[sayac] == shapes[s][1][0]) {
            // If the current point is the same as the first point of the current shape,
            // it means we have reached the end of the current shape.
            num_points[s] = i;
            s++; // Move on to the next shape
            i = 0; // Reset the index for the new shape
        }
        else {
            i++;
        }

        sayac++;
    }
}

void draw_polygon(SDL_Renderer* renderer, SDL_Point* points, int point_count) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLines(renderer, points, point_count);
    SDL_RenderDrawLine(renderer, points[point_count - 1].x, points[point_count - 1].y, points[0].x, points[0].y);
}

void fill_polygon(SDL_Renderer* renderer, SDL_Point* points, int point_count) {
    int minY = WINDOW_HEIGHT;
    int maxY = 0;
    for (int i = 0; i < point_count; i++) {
        if (points[i].y < minY)
            minY = points[i].y;
        if (points[i].y > maxY)
            maxY = points[i].y;
    }

    for (int y = minY; y <= maxY; y++) {
        std::vector<int> intersections;

        for (int i = 0; i < point_count; i++) {
            int j = (i + 1) % point_count;
            if ((points[i].y < y && points[j].y >= y) || (points[j].y < y && points[i].y >= y)) {
                int x = points[i].x + (y - points[i].y) * (points[j].x - points[i].x) / (points[j].y - points[i].y);
                intersections.push_back(x);
            }
        }

        std::sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i < intersections.size(); i += 2) {
            int x1 = max(0, intersections[i]);
            int x2 = min(WINDOW_WIDTH - 1, intersections[i + 1]);

            for (int x = x1; x <= x2; x++) {
                SDL_RenderDrawPoint(renderer, x, y);

                for (int i = 0; i < WINDOW_WIDTH; i += GRID_SIZE) {
                    for (int j = 0; j < WINDOW_HEIGHT; j += GRID_SIZE) {
                        if (x >= i && x < i + GRID_SIZE && y > j && y < j + GRID_SIZE) {
                            SDL_SetRenderDrawColor(renderer, 255, 191, 0, 255);
                            SDL_Rect rect = { i, j, GRID_SIZE, GRID_SIZE };
                            SDL_RenderFillRect(renderer, &rect);
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        }
                    }
                }
            }
        }
    }
}

double calculate_polygon_area(SDL_Point* points, int point_count) {
    double area = 0.0;
    int j = point_count - 1;

    for (int i = 0; i < point_count; i++) {
        area += (points[j].x + points[i].x) * (points[j].y - points[i].y);
        j = i;
    }

    return fabs(area / 2.0);
}

SDL_Rect calculate_enclosing_square(SDL_Point* points, int point_count) {
    int minX = INT_MAX, minY = INT_MAX;
    int maxX = INT_MIN, maxY = INT_MIN;

    for (int i = 0; i < point_count; i++) {
        if (points[i].x < minX) minX = points[i].x;
        if (points[i].y < minY) minY = points[i].y;
        if (points[i].x > maxX) maxX = points[i].x;
        if (points[i].y > maxY) maxY = points[i].y;
    }

    int width = maxX - minX;
    int height = maxY - minY;

    // Kare boyutunu, genişlik ve yüksekliğin maksimum değeri olarak ayarla
    int square_size = fmax(width, height) + 2 * GRID_SIZE; // GRID_SIZE eklendi

    // Kare merkezini, poligonun merkezi olarak ayarla
    int centerX = (minX + maxX) / 2; // GRID_SIZE / 2 çıkarıldı
    int centerY = (minY + maxY) / 2; // GRID_SIZE / 2 çıkarıldı

    SDL_Rect enclosing_square = { centerX - square_size / 2, centerY - square_size / 2, square_size, square_size };
    return enclosing_square;
}


Uint32 getpixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16*)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32*)p;
        break;

    default:
        return 0; // Uygun olmayan tür için 0 döndürüldü
    }
}

int** create_matrix(SDL_Surface* surface, SDL_Rect square) {
    int rows = square.h / GRID_SIZE + 2;
    int cols = square.w / GRID_SIZE + 2;

    // Add one extra row and column on all sides
    rows++;
    cols++;

    // Allocate memory for the matrix
    int** matrix = (int**)malloc((rows + 1) * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc((cols + 1) * sizeof(int));
    }

    // Initialize the matrix with zeros
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0;
        }
    }

    // Fill the matrix with pixel values
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int x = square.x + j * GRID_SIZE + 5;
            int y = square.y + i * GRID_SIZE + 5;

            // Check if the pixel is within the bounds of the surface
            if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
                // Get the pixel value
                Uint32 pixel = getpixel(surface, x, y);
                Uint8 r, g, b;
                SDL_GetRGB(pixel, surface->format, &r, &g, &b);
                if (r != 0 && g != 0 && b != 0) matrix[i][j] = 0;
                else matrix[i][j] = 1;
            }
        }
    }

    return matrix;
}






void draw_enclosing_square(SDL_Renderer* renderer, SDL_Rect square) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 128, 100);
    SDL_RenderDrawRect(renderer, &square);
}

void kontrol_et(int** matrix, SDL_Rect square) {
    for (int i = 0; i < square.h / GRID_SIZE; i++) {
        for (int j = 0; j < square.w / GRID_SIZE; j++) {
            if (matrix[i][j] == 1) {

                // En büyük kare boyutunu bul
                int kare_boyutu = 1;
                while (i + kare_boyutu < square.h / GRID_SIZE && j + kare_boyutu < square.w / GRID_SIZE) {
                    bool kare_tamam = true;
                    int bir_sayisi = 0; // hücre değeri 1 olanları saymak için sayaç
                    for (int m = i; m <= i + kare_boyutu; m++) {
                        for (int n = j; n <= j + kare_boyutu; n++) {
                            if (matrix[m][n] == 1) {
                                bir_sayisi++; // hücre değeri 1 olanları say
                            }
                            int kare_alan = kare_boyutu * kare_boyutu * 0.75;
                            if (matrix[m][n] != 1 && bir_sayisi < kare_alan) {
                                kare_tamam = false;
                                break;
                            }
                        }
                        if (!kare_tamam) {
                            break;
                        }
                    }
                    if (kare_tamam) {
                        kare_boyutu++;
                    }
                    else {
                        break;
                    }
                }
                kare_boyutu--; // son artışı geri al





                // Kareyi doldur
                int kare_boyu = kare_boyutu;
                //int matriste_gelecek_deger = kare_boyutu; // 2'den başlayarak kare boyutunu kodluyoruz
                if (kare_boyu >= 1 && kare_boyu < 2) kare_boyu = 1;
                else if (kare_boyu >= 2 && kare_boyu < 4) kare_boyu = 2;
                else if (kare_boyu >= 4 && kare_boyu < 8) kare_boyu = 4;
                else if (kare_boyu >= 8 && kare_boyu < 16) kare_boyu = 8;
                else kare_boyu = 16;
                for (int m = i; m < i + kare_boyu; m++) {
                    for (int n = j; n < j + kare_boyu; n++) {
                        matrix[m][n] = kare_boyu;
                    }
                }

                // Büyük bir kare bulduysak, bu satırı bitir ve diğer satıra geç
                if (kare_boyutu > 1) {
                    j += kare_boyutu - 1; // j'yi kare boyutu kadar ilerlet
                }
            }
        }
    }
}


void matrix_cizim(int** matrix, SDL_Rect square, SDL_Renderer* renderer2, int offset_x) {
    int satir = square.w, sutun = square.h;
    for (int i = 0; i < sutun / GRID_SIZE; i++) {
        for (int j = 0; j < satir / GRID_SIZE; j++) {
            switch (matrix[i][j]) {
            case 0:
                SDL_SetRenderDrawColor(renderer2, 255, 255, 255, 255); // Beyaz
                break;
            case 1:
                SDL_SetRenderDrawColor(renderer2, 255, 0, 0, 255); // Kırmızı
                break;
            case 2:
                SDL_SetRenderDrawColor(renderer2, 0, 255, 0, 255); // Yeşil
                break;
            case 4:
                SDL_SetRenderDrawColor(renderer2, 0, 0, 255, 255); // Mavi
                break;
            default:
                SDL_SetRenderDrawColor(renderer2, 100, 0, 0, 255); // Siyah
                break;
            }

            SDL_Rect rect;
            rect.x = j * 10 + offset_x;
            rect.y = i * 10 + 20;
            rect.w = 10;
            rect.h = 10;
            SDL_RenderFillRect(renderer2, &rect);
        }
    }
    SDL_RenderPresent(renderer2);
}
int maliyet_hesabi(int** matrix, SDL_Rect square, int platform, int sondaj, int alan) {
    int bir_sayaci = 0;
    int iki_sayaci = 0;
    int dort_sayaci = 0;
    int sekiz_sayaci = 0;
    int onalti_sayaci = 0;
    int otuziki_sayaci = 0;
    int altmisdort_sayaci = 0;

    for (int i = 0; i < square.h / GRID_SIZE; i++) {
        for (int j = 0; j < square.w / GRID_SIZE; j++) {
            switch (matrix[i][j]) {
            case 1:
                bir_sayaci++;
                break;
            case 2:
                iki_sayaci++;
                break;
            case 4:
                dort_sayaci++;
                break;
            case 8:
                sekiz_sayaci++;
                break;
            case 16:
                onalti_sayaci++;
                break;
            case 32:
                otuziki_sayaci++;
                break;
            case 64:
                altmisdort_sayaci++;
                break;
            default:
                break;
            }
        }
    }
    printf_s("%d  %d", sondaj, platform);
    int platform_sayisi = bir_sayaci + (iki_sayaci / 4) + (dort_sayaci / 16) + (sekiz_sayaci / 64) + (onalti_sayaci / 256) + (otuziki_sayaci / 1024) + (altmisdort_sayaci / 6096);
    int maliyet = (alan * sondaj) + (platform * platform_sayisi);
    return maliyet;
}





int main() {
    float toplam = 0;
    int platform, sondaj;
    printf_s("Cizdirmek istediginiz satir numarasini giriniz:");
    satir_no = getchar();
    txt(satir_no);

    printf_s("Platform fiyatını giriniz:");
    scanf_s("%d", &platform);

    bool kontrol = TRUE;
    while (kontrol) {
        printf_s("sondaj fiyatını giriniz(1-10 arasında bir fiyat giriniz):");
        scanf_s("%d", &sondaj);
        if (sondaj <= 10 && sondaj > 0) kontrol = FALSE;
        else kontrol = TRUE;
    }


    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Çokgen Çizimi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < WINDOW_WIDTH; i += GRID_SIZE) {
        for (int j = 0; j < WINDOW_HEIGHT; j += GRID_SIZE) {
            SDL_Rect rect = { i, j, GRID_SIZE, GRID_SIZE };
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
    for (int i = 0; i < WINDOW_WIDTH; i += GRID_SIZE) {
        for (int j = 0; j < WINDOW_HEIGHT; j += GRID_SIZE) {
            SDL_Rect rect = { i, j, GRID_SIZE, GRID_SIZE };
            SDL_RenderDrawRect(renderer, &rect);
        }
    }


    int shapes[3][2][MAX_POINTS]; // shape1_x/y, shape2_x/y and shape3_x/y
    int num_points[3]; // Number of points in each shape

    split_polygons(x, y, shapes, num_points);

    for (int s = 0; s < 2; s++) { // Loop over each shape
        if (num_points[s] < 3) break;
        SDL_Point* polygon_points = (SDL_Point*)malloc(num_points[s] * sizeof(SDL_Point));
        if (polygon_points == NULL) {
            break;
        }

        for (int i = 0; i < num_points[s]; i++) {
            polygon_points[i].x = shapes[s][0][i] * GRID_SIZE;
            polygon_points[i].y = shapes[s][1][i] * GRID_SIZE;
        }

        fill_polygon(renderer, polygon_points, num_points[s]);
        double polygon_area = calculate_polygon_area(polygon_points, num_points[s]);

        for (int s = 0; s < 2; s++) {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
            for (int i = 0; i < WINDOW_WIDTH; i += GRID_SIZE) {
                for (int j = 0; j < WINDOW_HEIGHT; j += GRID_SIZE) {
                    SDL_Rect rect = { i, j, GRID_SIZE, GRID_SIZE };
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }
        }
        if (s < 2) {
            draw_polygon(renderer, polygon_points, num_points[s]);


        }
        /*SDL_Rect enclosing_square = calculate_enclosing_square(polygon_points, num_points[s], polygon_area);
        draw_enclosing_square(renderer, enclosing_square);
        */
        printf("%d. Cokgen Alanı: %.2f\n", s + 1, polygon_area / 100);

        SDL_RenderPresent(renderer);
        printf_s("%d. Kaynak rezerv degeri: %.2f\n", s + 1, polygon_area / 10);
        toplam = polygon_area + toplam;



        free(polygon_points); // Don't forget to free the allocated memory
    }

    int width, height;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    SDL_Surface* screenSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, screenSurface->pixels, screenSurface->pitch);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window2 = SDL_CreateWindow("Çokgen Kareleme", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer2 = SDL_CreateRenderer(window2, -1, 0);

    SDL_SetRenderDrawColor(renderer2, 255, 255, 255, 255);
    for (int i = 0; i < WINDOW_WIDTH; i += GRID_SIZE) {
        for (int j = 0; j < WINDOW_HEIGHT; j += GRID_SIZE) {
            SDL_Rect rect = { i, j, GRID_SIZE, GRID_SIZE };
            SDL_RenderFillRect(renderer2, &rect);
        }
    }


    int offset_x = 0;
    for (int s = 0; s < 2; s++) {
        if (num_points[s] < 3) break;
        printf("\n\n\n");
        SDL_Point* polygon_points = (SDL_Point*)malloc(num_points[s] * sizeof(SDL_Point));
        if (polygon_points == NULL) {
            break;
        }
        for (int i = 0; i < num_points[s]; i++) {
            polygon_points[i].x = shapes[s][0][i] * GRID_SIZE;
            polygon_points[i].y = shapes[s][1][i] * GRID_SIZE;
        }
        double polygon_area = calculate_polygon_area(polygon_points, num_points[s]);
        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
        SDL_Rect enclosing_square = calculate_enclosing_square(polygon_points, num_points[s]);
        draw_enclosing_square(renderer, enclosing_square);
        SDL_Rect square = calculate_enclosing_square(polygon_points, num_points[s]);
        int** matrix = create_matrix(screenSurface, square);
        int alan = 0;
        for (int i = 0; i < square.h / GRID_SIZE; i++) {
            for (int j = 0; j < square.w / GRID_SIZE; j++) {
                printf("%d ", matrix[i][j]);
                if (matrix[i][j] == 1) alan++;
            }
            printf("\n");
        }
        printf("%d\n", alan);
        
        kontrol_et(matrix, square);
        for (int i = 0; i < square.h / GRID_SIZE; i++) {
            for (int j = 0; j < square.w / GRID_SIZE; j++) {
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }
        offset_x += square.w;
        matrix_cizim(matrix, square, renderer2, offset_x);
        int maliyet = maliyet_hesabi(matrix, square, platform, sondaj, alan);
        printf_s("toplam maliyet: %d", maliyet);
    }

    SDL_SetRenderDrawColor(renderer2, 128, 128, 128, 50);
    for (int i = 0; i < WINDOW_WIDTH; i += GRID_SIZE) {
        for (int j = 0; j < WINDOW_HEIGHT; j += GRID_SIZE) {
            SDL_Rect rect = { i, j, GRID_SIZE, GRID_SIZE };
            SDL_RenderDrawRect(renderer2, &rect);
        }
    }
    SDL_RenderPresent(renderer2);
    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }

    return 0;
}