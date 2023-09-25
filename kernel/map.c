#include "character.h"
#include "map.h"
#include "wall_block.h"
#include "../uart/uart1.h"
#include "stat.h"

void draw_map_from_array(int map[][28])
{
    for (int i = 0; i < 17; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            if (map[i][j] > 0)
            {
                drawGameAsset(map[i][j] - 1, j * block_width, i * block_height, block_width, block_height, wall_block_allArray);
            }
        }
    }
}

void draw_stats(int heart_count) {
    int spacing = 5; // Spacing between each heart
    int heart_width = 30; // Width of the heart icon
    for (int i = 0; i < heart_count; i++) {
        int x_position = i * (heart_width + spacing);
        drawGameAsset(0, x_position, 0, heart_width, 30, heart_iconallArray);
    }
}
