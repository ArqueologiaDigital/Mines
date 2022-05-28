//TODO: mover para um common-header:
void platform_init();
void draw_minefield();
void shutdown();
void wait_for_any_key();


void main(){
	platform_init();
	draw_minefield();
	wait_for_any_key();
	shutdown();
}
