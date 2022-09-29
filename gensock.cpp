#include "stdlib.h"
#include "unistd.h"
#include "math.h"

int inject_bind(int index, const char* filesock);
int inject_window(int index, int stream, int x, int y, int sx, int sy);
int inject_send(int index, const void* data, int size);

int main() {
    int ninj = 0;
    inject_bind(ninj, "inj.sock");
    int sx = 128;
    int sy = 128;
    int px = 4;
    int py = 16;
    inject_window(ninj, 0, px, py, sx, sy);

    char* frame = (char*)malloc(sx*sy*4);
    int sx2 = sx / 2;
    int sy2 = sy / 2;
    double sxq = sx2 * sx2;
    double syq = sy2 * sy2;

    int tone = 1000 / 60;
    for (int nc = 0; ; nc++) {
        //  Имитатор игры
        for (int y = 0; y < sy; y++) {
            for (int x = 0; x < sx; x++) {
                int pos = (y * sx + x) * 4;
                float rad = sqrt((x - sx2) * (x - sx2) / sxq + (y - sy2) * (y - sy2) / syq);
                int cr = 0, cg = 0, cb = 0, ca = 0;
                if (rad > 0.33 && rad < 0.99) {
                    float sn = (rad - 0.66) * 3;
                    ca = (sn >= 0) ? 255 - sn * 255 : 255 + sn * 255;
                    float tor = asin(sn) * 64 / M_PI - nc * 0.1;
                    float rot = atan2(y - sy2 + 0.5, x - sx2 + 0.5) * 64 / M_PI;
                    cr = ((int)(tor + rot) & 0xf) / 8 * 255;
                    cg = ((int)(tor - rot) & 0xf) / 8 * 255;
                    cb = ((int)tor & 0xf) / 8 * 255;
                }
                frame[pos + 0] = cr;
                frame[pos + 1] = cg;
                frame[pos + 2] = cb;
                frame[pos + 3] = ca;
            }
        }
        //  Передача кадра
        inject_send(ninj, frame, sx*sy*4);

        usleep(tone * 1000);
    }
}

