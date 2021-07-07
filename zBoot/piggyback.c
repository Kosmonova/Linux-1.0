/*
 *	linux/zBoot/piggyback.c
 *
 *	(C) 1993 Hannu Savolainen
 */

/*
 *	This program reads the compressed system image from stdin and
 *	encapsulates it into an object file written to the stdout.
 */


// https://docs.oracle.com/cd/E19683-01/816-1386/chapter6-79797/index.html
// https://linux.die.net/man/5/elf
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
// https://linux-audit.com/elf-binaries-on-linux-understanding-and-analysis/
// https://linuxhint.com/understanding_elf_file_format/
// http://m68hc11.serveftp.org/ELF/ch4.symtab.html

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <elf.h>

#define SYMBOLS_NUMBER			4
#define NR_SYMBOL_DATA 			1
#define NR_SYMBOL_INPUT_LEN		2
#define NR_SYMBOL_INPUT_DATA	3

#define FIRST_LOCAL_SYMBOL (NR_SYMBOL_INPUT_LEN)

#define SECTIONS_NUMBER			5
#define NR_SECTION_DATA 		1
#define NR_SECTION_SHSTRTAB 	2
#define NR_SECTION_SYMTAB		3
#define NR_SECTION_STRTAB 		4


int offset_symbol = 0;

void cat_symbol(char **buff, char *name_symbol, Elf32_Shdr *pshr)
{
	int size_name = strlen(name_symbol) + 1;
	memcpy(*buff, name_symbol, size_name);
	pshr->sh_name = offset_symbol;
	offset_symbol += size_name;
	*buff += size_name;
}

int main(int argc, char *argv[])
{
	int c, n=0, len=0;
	char tmp_buf[512*1024];

	Elf32_Ehdr *elfn = (Elf32_Ehdr*)tmp_buf;

	*elfn =
	(Elf32_Ehdr){
		.e_ident=
		{
			0x7F, 0x45, 0x4C, 0x46, 
			0x01, 0x01, 0x01, 0x00, 
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x01
		},
		.e_type = 0x01,
		.e_machine = 0x03,
		.e_version = 0x01,
		.e_entry = 0x00,
		.e_phoff = 0x00,
		.e_shoff = 0xA4,
		.e_flags = 0,
		.e_ehsize = 0x34,
		.e_phentsize = 0x00,
		.e_phnum = 0x00,
		.e_shentsize = 0x28,
		.e_shnum = SECTIONS_NUMBER,
		.e_shstrndx = NR_SECTION_SHSTRTAB,
	};

	len = sizeof(Elf32_Ehdr);
	int offset_data = len;
	int *input_len = tmp_buf + len;
	len += sizeof(int);

	while ((n = read(0, &tmp_buf[len], sizeof(tmp_buf)-len+1)) > 0)
	      len += n;

	if (n==-1)
	{
		perror("stdin");
		exit(-1);
	}

	if (len >= sizeof(tmp_buf))
	{
		fprintf(stderr, "%s: Input too large\n", argv[0]);
		exit(-1);
	}

	*input_len = len -sizeof(Elf32_Ehdr) - sizeof(int);

	fprintf(stderr, "Compressed size %d.\n", len);

	Elf32_Shdr shdr[SECTIONS_NUMBER];
	memset(shdr, 0, sizeof(Elf32_Shdr) * SECTIONS_NUMBER);

	char *ptmp_buf = tmp_buf + len;
	cat_symbol(&ptmp_buf , "", shdr);
	cat_symbol(&ptmp_buf, ".symtab", shdr + NR_SECTION_SYMTAB);
	cat_symbol(&ptmp_buf, ".strtab", shdr + NR_SECTION_STRTAB);
	cat_symbol(&ptmp_buf, ".shstrtab", shdr + NR_SECTION_SHSTRTAB);
	cat_symbol(&ptmp_buf, ".data", shdr + NR_SECTION_DATA);

	int len_data = len - offset_data;
	len += offset_symbol;
	elfn->e_shoff = len;
	Elf32_Shdr *pshdr = tmp_buf + len;

	memcpy(pshdr, shdr, sizeof(Elf32_Shdr) * SECTIONS_NUMBER);

	pshdr[NR_SECTION_DATA].sh_type = SHT_PROGBITS;
	pshdr[NR_SECTION_DATA].sh_flags = SHF_ALLOC | SHF_WRITE;
	pshdr[NR_SECTION_DATA].sh_offset = offset_data;
	pshdr[NR_SECTION_DATA].sh_size = len_data;
	pshdr[NR_SECTION_DATA].sh_addralign = 1;

	pshdr[NR_SECTION_SHSTRTAB].sh_type = SHT_STRTAB;
	pshdr[NR_SECTION_SHSTRTAB].sh_offset = len - offset_symbol;
	pshdr[NR_SECTION_SHSTRTAB].sh_size = offset_symbol;
	pshdr[NR_SECTION_SHSTRTAB].sh_addralign = 1;

	len += sizeof(Elf32_Shdr) * SECTIONS_NUMBER;

	pshdr[NR_SECTION_SYMTAB].sh_type = SHT_SYMTAB;
	pshdr[NR_SECTION_SYMTAB].sh_offset = len;
	pshdr[NR_SECTION_SYMTAB].sh_size = sizeof(Elf32_Sym) * SYMBOLS_NUMBER;
	pshdr[NR_SECTION_SYMTAB].sh_link = NR_SECTION_STRTAB;
	pshdr[NR_SECTION_SYMTAB].sh_info = FIRST_LOCAL_SYMBOL;
	pshdr[NR_SECTION_SYMTAB].sh_addralign = 4;
	pshdr[NR_SECTION_SYMTAB].sh_entsize = 0x10;

	pshdr[NR_SECTION_STRTAB].sh_type = SHT_STRTAB;

	Elf32_Sym *psymbols = tmp_buf + len;
	len += sizeof(Elf32_Sym) * SYMBOLS_NUMBER;
	memset(psymbols, 0, sizeof(Elf32_Sym) * SYMBOLS_NUMBER);

	psymbols[NR_SYMBOL_DATA].st_shndx = NR_SECTION_DATA;
	psymbols[NR_SYMBOL_DATA].st_info = ELF32_ST_INFO(STB_LOCAL, STT_SECTION);

	psymbols[NR_SYMBOL_INPUT_LEN].st_shndx = NR_SECTION_DATA;
	psymbols[NR_SYMBOL_INPUT_LEN].st_info = ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE);

	psymbols[NR_SYMBOL_INPUT_DATA].st_shndx = NR_SECTION_DATA;
	psymbols[NR_SYMBOL_INPUT_DATA].st_info = ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE);
	psymbols[NR_SYMBOL_INPUT_DATA].st_value = sizeof(int);

	char string_names[] = {"input_data\0input_len\0"};
	pshdr[NR_SECTION_STRTAB].sh_offset = len;
	pshdr[NR_SECTION_STRTAB].sh_size = sizeof(string_names);
	tmp_buf[len++] = 0;

	psymbols[NR_SYMBOL_INPUT_LEN].st_name = 2 + strlen(string_names);
	psymbols[NR_SYMBOL_INPUT_DATA].st_name = 1;

	write(1, tmp_buf, len);
	write(1, string_names, sizeof(string_names));

	exit(0);
}
