#include<stdio.h>
int main(){
    int pages[]={7, 0, 1, 2, 0, 3, 0};
    int n=sizeof(pages)/sizeof(pages[0]);
    //printf("Number of pages: %d\n", n);
    int frame_size=3;
    int frame[frame_size];
    //printf("Frame size: %d\n", frame_size);
    if (frame[0] == 0 && frame[1] == 0 && frame[2] == 0) {
        for (int i = 0; i < frame_size; i++) {
            frame[i] = -1;
        }
    }
   for (int i = 0; i < n; i++) {
        int page = pages[i];
        int found = 0;
        for (int j = 0; j < frame_size; j++) {
            if (frame[j] == page) {
                found = 1;
                break;
            }
        }
        if (!found) {
            int lru_index = 0;
            for (int j = 1; j < frame_size; j++) {
                if (frame[j] < frame[lru_index]) {
                    lru_index = j;
                }
            }
            frame[lru_index] = page;
        }
    

    for (int i = 0; i < frame_size; i++) {
        printf("%d ", frame[i]);
        printf("\n");
    }

}
    
    return 0;

}
