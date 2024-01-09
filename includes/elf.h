#ifndef __ELF_H__
#define __ELF_H__

# include <stdint.h>

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_Sword;

//type of sections
# define SHT_NULL 0
# define SHT_PROGBITS 1
# define SHT_SYMTAB 2
# define SHT_STRTAB 3
# define SHT_RELA 4
# define SHT_HASH 5
# define SHT_DYNAMIC 6
# define SHT_NOTE 7
# define SHT_NOBITS 8
# define SHT_REL 9
# define SHT_SHLIB 10
# define SHT_DYNSYM 11
# define SHT_LOPROC 0x70000000
# define SHT_HIPROC 0x7fffffff
# define SHT_LOUSER 0x80000000
# define SHT_HIUSER 0xffffffff

typedef struct
{
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct
{
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

#endif
