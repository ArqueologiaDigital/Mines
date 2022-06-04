#include "pixo_background-1.xpm"
#include <stdio.h>

#define bool char
#define true 0xFF
#define false 0x00

#define MAX_TILES 512
#define WIDTH 32*8
#define HEIGHT 32*8
#define NUM_COLORS 64
#define xpm_header_offset (1+NUM_COLORS)

struct gfx_layout
{
	int width;              // pixel width of each element
	int height;             // pixel height of each element
	int total;              // total number of elements
	int planes;             // number of bitplanes
	int planeoffset[4]; // bit offset of each bitplane
	int xoffset[32]; // bit offset of each horizontal pixel
	int yoffset[32]; // bit offset of each vertical pixel
	int increment;      // distance between two consecutive elements (in bits)
};

static const struct gfx_layout tilelayout =
{
	32,32,  /* 32*32 tiles */
	512,    /* 512 tiles */
	4,      /* 4 bits per pixel */
	{ 512*256*8+4, 512*256*8+0, 4, 0 },
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3,
			64*8+0, 64*8+1, 64*8+2, 64*8+3, 65*8+0, 65*8+1, 65*8+2, 65*8+3,
			128*8+0, 128*8+1, 128*8+2, 128*8+3, 129*8+0, 129*8+1, 129*8+2, 129*8+3,
			192*8+0, 192*8+1, 192*8+2, 192*8+3, 193*8+0, 193*8+1, 193*8+2, 193*8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16,
			16*16, 17*16, 18*16, 19*16, 20*16, 21*16, 22*16, 23*16,
			24*16, 25*16, 26*16, 27*16, 28*16, 29*16, 30*16, 31*16 },
	256*8   /* every tile takes 256 consecutive bytes */
};

FILE* tilemap;
FILE* tile_rom_fp[8];

void save_tile(int x, int y, int tile_number){
	printf("save_tile x:%d y:%d tile_number:%d\n", x, y, tile_number);

	char color_code;
	char color_index;
	char value;

	//store this tile in the tilemap
	value = tile_number;
	fseek(tilemap, (8*x + y)*2, SEEK_SET);
	fwrite(&value, 1, 1, tilemap);

	for (int i=0; i<32; i++){
		for (int j=0; j<32; j++){

//Essa era a linha que deveria ser a correta:
//			color_code = pixo_background[xpm_header_offset + y*32 + j][x*32 + i];

//Entretanto tive que fazer uma massagem algébrica gambiarrística pra acertar o posicionamento dos pixels
//Não está claro ainda pra mim por que... Eu gostaria de implementar isso de uma forma mais clara e mais intuitiva:
			color_code = pixo_background[xpm_header_offset + y*32 + j][x*32 + (i/4)*4 + 3-(i%4)];


			//find the color index by searching in the XPM palette header
			color_index = 0;
			for (int idx=0; idx<64; idx++){
				if (color_code == pixo_background[idx+1][0]){
					color_index=idx;
					break;
				}
			}

			for (int bit=0; bit<4; bit++){
				int bit_address = tilelayout.increment * tile_number
                        + tilelayout.planeoffset[bit]
                        + tilelayout.xoffset[i] + tilelayout.yoffset[j];

//				printf("0x%08X 0x%08X [%d]\n", bit_address, bit_address/8, (bit_address/8)/0x8000);
				FILE* fp = tile_rom_fp[(bit_address/8)/0x8000];
				fseek(fp, (bit_address/8)%0x8000, SEEK_SET);
				fread(&value, 1, 1, fp);

				if (color_index & (1<<bit)){
					value |= (1<<(bit_address%8));
				}else{
					value &= ~(1<<(bit_address%8));
				}
				fseek(fp, (bit_address/8)%0x8000, SEEK_SET);
				fwrite(&value, 1, 1, fp);
			}
		}
	}
}

int calculate_tile_hash(int i, int j){
	//TODO: implement-me!
	static int fake_incremental_hash = 0;
	return fake_incremental_hash++;
}

int main(){
	tilemap = fopen("11c_gs14.bin", "r+");

	tile_rom_fp[0] = fopen("06c_gs13.bin", "r+");
	tile_rom_fp[1] = fopen("05c_gs12.bin", "r+");
	tile_rom_fp[2] = fopen("04c_gs11.bin", "r+");
	tile_rom_fp[3] = fopen("02c_gs10.bin", "r+");
	tile_rom_fp[4] = fopen("06a_gs09.bin", "r+");
	tile_rom_fp[5] = fopen("05a_gs08.bin", "r+");
	tile_rom_fp[6] = fopen("04a_gs07.bin", "r+");
	tile_rom_fp[7] = fopen("02a_gs06.bin", "r+");

	int tile_number=0;
	int hash;
	int tile_hashes[MAX_TILES];

	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++){

			hash = calculate_tile_hash(i, j);
			bool found_tile = false;
			for (int k=0; k<tile_number; k++){
				if (hash == tile_hashes[k]){
					found_tile = true;
					break;
				}
			}

			if (!found_tile){
				tile_hashes[tile_number] = hash;
				save_tile(i, j, tile_number++);
			}

			if (tile_number > MAX_TILES){
				fclose(tilemap);
				for (int i=0; i<8; i++)
					fclose(tile_rom_fp[i]);
				return -1;
			}
		}
	}

	fclose(tilemap);
	for (int i=0; i<8; i++)
		fclose(tile_rom_fp[i]);
	return 0;
}

