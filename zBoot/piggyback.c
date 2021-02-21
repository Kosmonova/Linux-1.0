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
		.e_shnum = 0x07,
		.e_shstrndx = 0x04,
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

	Elf32_Shdr shdr[7];
	memset(shdr, 0, sizeof(Elf32_Shdr) * 7);

	char *ptmp_buf = tmp_buf + len;
	cat_symbol(&ptmp_buf , "", shdr);
	cat_symbol(&ptmp_buf, ".symtab", shdr +5);
	cat_symbol(&ptmp_buf, ".strtab", shdr + 6);
	cat_symbol(&ptmp_buf, ".shstrtab", shdr + 4);
	cat_symbol(&ptmp_buf, ".text", shdr + 1);
	cat_symbol(&ptmp_buf, ".data", shdr + 2);
	cat_symbol(&ptmp_buf, ".bss", shdr + 3);

	int len_data = len - offset_data;
	len += offset_symbol;
	elfn->e_shoff = len;
	Elf32_Shdr *pshdr = tmp_buf + len;

	memcpy(pshdr, shdr, sizeof(Elf32_Shdr) * 7);

	pshdr[1].sh_type = SHT_PROGBITS;
	pshdr[1].sh_flags = SHF_ALLOC | SHF_EXECINSTR;
	pshdr[1].sh_offset = offset_data;
	pshdr[1].sh_addralign = 1;

	pshdr[2].sh_type = SHT_PROGBITS;
	pshdr[2].sh_flags = SHF_ALLOC | SHF_WRITE;
	pshdr[2].sh_offset = offset_data;
	pshdr[2].sh_size = len_data;
	pshdr[2].sh_addralign = 1;

	pshdr[3].sh_type = SHT_NOBITS;
	pshdr[3].sh_flags = SHF_ALLOC | SHF_WRITE;
	pshdr[3].sh_offset = len - offset_symbol;
	pshdr[3].sh_addralign = 1;

	pshdr[4].sh_type = SHT_STRTAB;
	pshdr[4].sh_offset = len - offset_symbol;
	pshdr[4].sh_size = offset_symbol;
	pshdr[4].sh_addralign = 1;

	len += sizeof(Elf32_Shdr) * 7;

	pshdr[5].sh_type = SHT_SYMTAB;
	pshdr[5].sh_offset = len;
	pshdr[5].sh_size = sizeof(Elf32_Sym) * 6;
	pshdr[5].sh_link = 6;
	pshdr[5].sh_info = 4;
	pshdr[5].sh_addralign = 4;
	pshdr[5].sh_entsize = 0x10;

	pshdr[6].sh_type = SHT_STRTAB;

	Elf32_Sym *psymbols = tmp_buf + len;
	len += sizeof(Elf32_Sym) * 6;
	memset(psymbols, 0, sizeof(Elf32_Sym) * 6);
	psymbols[1].st_shndx = 1;
	psymbols[1].st_info = ELF32_ST_INFO(STB_LOCAL, STT_SECTION);

	psymbols[2].st_shndx = 2;
	psymbols[2].st_info = ELF32_ST_INFO(STB_LOCAL, STT_SECTION);

	psymbols[3].st_shndx = 3;
	psymbols[3].st_info = ELF32_ST_INFO(STB_LOCAL, STT_SECTION);

	psymbols[4].st_shndx = 2;
	psymbols[4].st_info = ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE);

	psymbols[5].st_shndx = 2;
	psymbols[5].st_info = ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE);
	psymbols[5].st_value = sizeof(int);

	char string_names[] = {"input_data\0input_len\0"};
	pshdr[6].sh_offset = len;
	pshdr[6].sh_size = sizeof(string_names);
	tmp_buf[len++] = 0;

	psymbols[4].st_name = 2 + strlen(string_names);
	psymbols[5].st_name = 1;

	write(1, tmp_buf, len);
	write(1, string_names, sizeof(string_names));

	exit(0);
}
