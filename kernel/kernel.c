#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "You need to compile with an ix86-elf compiler"
#endif

enum vga_color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_GREEN);
	terminal_buffer = (uint16_t *)0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
	if (c == '\n')
	{
		++terminal_row;
		return;
	}
	if (c == '\t')
	{
		terminal_column += 4;
		return;
	}
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH)
	{
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void terminal_write(const char *data)
{
	size_t size = strlen(data);
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
	return;
}
void terminal_write_center(const char *data)
{
	size_t text_size = strlen(data);
	if(terminal_column!=0)
		terminal_putchar('\n');
	terminal_column = (VGA_WIDTH - text_size) / 2;
	for (size_t i = 0; i < text_size; i++)
	{
		terminal_putchar(data[i]);
	}
	terminal_putchar('\n');
	return;
}

void terminal_draw_horizontal_line(const char c)
{
	terminal_column = 0;
	for (size_t i = 0; i < VGA_WIDTH; i++)
	{
		terminal_putchar(c);
	}
	return;
}

void set_cursor(size_t x, size_t y)
{
	terminal_column = x;
	terminal_row = y;
}
void draw_box(size_t x, size_t y, size_t height, size_t width, enum vga_color color)
{
	// Experimental
	height += y;
	width += x;
	uint8_t previous_terminal_colour = terminal_color;
	terminal_color = vga_entry_color(color, color);
	terminal_row = y; 
	while(terminal_row < height)
	{
		terminal_column = x;
		while(terminal_column < width)
		{
			terminal_putchar(' ');
		}
		terminal_putchar('\n');
	}
	terminal_color = previous_terminal_colour;
}

void kernel_main(void)
{
	terminal_initialize();
	terminal_draw_horizontal_line('-');
	terminal_write_center("Akatsuki OS 0.1");
	// set_cursor(terminal_column, terminal_row + 1); //Method signature set_cursor(x,y)
	terminal_write_center("Made with Love by Rajdeep Roy Chowdhury");
	terminal_draw_horizontal_line('-');
	draw_box(20, 7, 10, 40, VGA_COLOR_LIGHT_MAGENTA);
	set_cursor(0, 7);
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_LIGHT_MAGENTA));
	terminal_write_center("Welcome to Akatsuki...");
	
}