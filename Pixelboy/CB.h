#pragma once
#include "CPU.h"

struct ExtendedOpcode
{
	const char* sMnemonic;
	const void* function;
};

void Undefined(CPU* cpu);
uint8_t lastValue;

uint8_t SWAP(CPU* cpu, uint8_t value);
void BIT(CPU* cpu, uint8_t bit, uint8_t value);
uint8_t RL(CPU* cpu, uint8_t value);
uint8_t SLA(CPU* cpu, uint8_t value);
uint8_t SRL(CPU* cpu, uint8_t value);
uint8_t RR(CPU* cpu, uint8_t value);
uint8_t SET(CPU* cpu, uint8_t bit, uint8_t value);
uint8_t RLC(CPU* cpu, uint8_t value);
uint8_t RRC(CPU* cpu, uint8_t value);
uint8_t SRA(CPU* cpu, uint8_t value);

void SWAP_A(CPU* cpu);
void SWAP_B(CPU* cpu);
void SWAP_C(CPU* cpu);
void SWAP_D(CPU* cpu);
void SWAP_E(CPU* cpu);
void SWAP_H(CPU* cpu);
void SWAP_L(CPU* cpu);
void SWAP_HL(CPU* cpu);

void RES_0_A(CPU* cpu);
void RES_0_B(CPU* cpu);
void RES_0_C(CPU* cpu);
void RES_0_D(CPU* cpu);
void RES_0_E(CPU* cpu);
void RES_0_H(CPU* cpu);
void RES_0_L(CPU* cpu);
void RES_0_HL(CPU* cpu);

void RES_1_A(CPU* cpu);
void RES_1_B(CPU* cpu);
void RES_1_C(CPU* cpu);
void RES_1_D(CPU* cpu);
void RES_1_E(CPU* cpu);
void RES_1_H(CPU* cpu);
void RES_1_L(CPU* cpu);
void RES_1_HL(CPU* cpu);

void RES_2_A(CPU* cpu);
void RES_2_B(CPU* cpu);
void RES_2_C(CPU* cpu);
void RES_2_D(CPU* cpu);
void RES_2_E(CPU* cpu);
void RES_2_H(CPU* cpu);
void RES_2_L(CPU* cpu);
void RES_2_HL(CPU* cpu);

void RES_3_A(CPU* cpu);
void RES_3_B(CPU* cpu);
void RES_3_C(CPU* cpu);
void RES_3_D(CPU* cpu);
void RES_3_E(CPU* cpu);
void RES_3_H(CPU* cpu);
void RES_3_L(CPU* cpu);
void RES_3_HL(CPU* cpu);

void RES_4_A(CPU* cpu);
void RES_4_B(CPU* cpu);
void RES_4_C(CPU* cpu);
void RES_4_D(CPU* cpu);
void RES_4_E(CPU* cpu);
void RES_4_H(CPU* cpu);
void RES_4_L(CPU* cpu);
void RES_4_HL(CPU* cpu);

void RES_5_A(CPU* cpu);
void RES_5_B(CPU* cpu);
void RES_5_C(CPU* cpu);
void RES_5_D(CPU* cpu);
void RES_5_E(CPU* cpu);
void RES_5_H(CPU* cpu);
void RES_5_L(CPU* cpu);
void RES_5_HL(CPU* cpu);

void RES_6_A(CPU* cpu);
void RES_6_B(CPU* cpu);
void RES_6_C(CPU* cpu);
void RES_6_D(CPU* cpu);
void RES_6_E(CPU* cpu);
void RES_6_H(CPU* cpu);
void RES_6_L(CPU* cpu);
void RES_6_HL(CPU* cpu);

void RES_7_A(CPU* cpu);
void RES_7_B(CPU* cpu);
void RES_7_C(CPU* cpu);
void RES_7_D(CPU* cpu);
void RES_7_E(CPU* cpu);
void RES_7_H(CPU* cpu);
void RES_7_L(CPU* cpu);
void RES_7_HL(CPU* cpu);

void BIT_0_A(CPU* cpu);
void BIT_0_B(CPU* cpu);
void BIT_0_C(CPU* cpu);
void BIT_0_D(CPU* cpu);
void BIT_0_E(CPU* cpu);
void BIT_0_H(CPU* cpu);
void BIT_0_L(CPU* cpu);
void BIT_0_HL(CPU* cpu);

void BIT_1_A(CPU* cpu);
void BIT_1_B(CPU* cpu);
void BIT_1_C(CPU* cpu);
void BIT_1_D(CPU* cpu);
void BIT_1_E(CPU* cpu);
void BIT_1_H(CPU* cpu);
void BIT_1_L(CPU* cpu);
void BIT_1_HL(CPU* cpu);

void BIT_2_A(CPU* cpu);
void BIT_2_B(CPU* cpu);
void BIT_2_C(CPU* cpu);
void BIT_2_D(CPU* cpu);
void BIT_2_E(CPU* cpu);
void BIT_2_H(CPU* cpu);
void BIT_2_L(CPU* cpu);
void BIT_2_HL(CPU* cpu);

void BIT_3_A(CPU* cpu);
void BIT_3_B(CPU* cpu);
void BIT_3_C(CPU* cpu);
void BIT_3_D(CPU* cpu);
void BIT_3_E(CPU* cpu);
void BIT_3_H(CPU* cpu);
void BIT_3_L(CPU* cpu);
void BIT_3_HL(CPU* cpu);

void BIT_4_A(CPU* cpu);
void BIT_4_B(CPU* cpu);
void BIT_4_C(CPU* cpu);
void BIT_4_D(CPU* cpu);
void BIT_4_E(CPU* cpu);
void BIT_4_H(CPU* cpu);
void BIT_4_L(CPU* cpu);
void BIT_4_HL(CPU* cpu);

void BIT_5_A(CPU* cpu);
void BIT_5_B(CPU* cpu);
void BIT_5_C(CPU* cpu);
void BIT_5_D(CPU* cpu);
void BIT_5_E(CPU* cpu);
void BIT_5_H(CPU* cpu);
void BIT_5_L(CPU* cpu);
void BIT_5_HL(CPU* cpu);

void BIT_6_A(CPU* cpu);
void BIT_6_B(CPU* cpu);
void BIT_6_C(CPU* cpu);
void BIT_6_D(CPU* cpu);
void BIT_6_E(CPU* cpu);
void BIT_6_H(CPU* cpu);
void BIT_6_L(CPU* cpu);
void BIT_6_HL(CPU* cpu);

void BIT_7_A(CPU* cpu);
void BIT_7_B(CPU* cpu);
void BIT_7_C(CPU* cpu);
void BIT_7_D(CPU* cpu);
void BIT_7_E(CPU* cpu);
void BIT_7_H(CPU* cpu);
void BIT_7_L(CPU* cpu);
void BIT_7_HL(CPU* cpu);

void RL_B(CPU* cpu);
void RL_C(CPU* cpu);
void RL_D(CPU* cpu);
void RL_E(CPU* cpu);
void RL_H(CPU* cpu);
void RL_L(CPU* cpu);
void RL_HL(CPU* cpu);
void RL_A(CPU* cpu);

void SLA_A(CPU* cpu);
void SLA_B(CPU* cpu);
void SLA_C(CPU* cpu);
void SLA_D(CPU* cpu);
void SLA_E(CPU* cpu);
void SLA_H(CPU* cpu);
void SLA_L(CPU* cpu);
void SLA_HL(CPU* cpu);

void SRL_A(CPU* cpu);
void SRL_B(CPU* cpu);
void SRL_C(CPU* cpu);
void SRL_D(CPU* cpu);
void SRL_E(CPU* cpu);
void SRL_H(CPU* cpu);
void SRL_L(CPU* cpu);
void SRL_HL(CPU* cpu);

void RR_A(CPU* cpu);
void RR_B(CPU* cpu);
void RR_C(CPU* cpu);
void RR_D(CPU* cpu);
void RR_E(CPU* cpu);
void RR_H(CPU* cpu);
void RR_L(CPU* cpu);
void RR_HL(CPU* cpu);

void SET_0_A(CPU* cpu);
void SET_0_B(CPU* cpu);
void SET_0_C(CPU* cpu);
void SET_0_D(CPU* cpu);
void SET_0_E(CPU* cpu);
void SET_0_H(CPU* cpu);
void SET_0_L(CPU* cpu);
void SET_0_HL(CPU* cpu);

void SET_1_A(CPU* cpu);
void SET_1_B(CPU* cpu);
void SET_1_C(CPU* cpu);
void SET_1_D(CPU* cpu);
void SET_1_E(CPU* cpu);
void SET_1_H(CPU* cpu);
void SET_1_L(CPU* cpu);
void SET_1_HL(CPU* cpu);

void SET_2_A(CPU* cpu);
void SET_2_B(CPU* cpu);
void SET_2_C(CPU* cpu);
void SET_2_D(CPU* cpu);
void SET_2_E(CPU* cpu);
void SET_2_H(CPU* cpu);
void SET_2_L(CPU* cpu);
void SET_2_HL(CPU* cpu);

void SET_3_A(CPU* cpu);
void SET_3_B(CPU* cpu);
void SET_3_C(CPU* cpu);
void SET_3_D(CPU* cpu);
void SET_3_E(CPU* cpu);
void SET_3_H(CPU* cpu);
void SET_3_L(CPU* cpu);
void SET_3_HL(CPU* cpu);

void SET_4_A(CPU* cpu);
void SET_4_B(CPU* cpu);
void SET_4_C(CPU* cpu);
void SET_4_D(CPU* cpu);
void SET_4_E(CPU* cpu);
void SET_4_H(CPU* cpu);
void SET_4_L(CPU* cpu);
void SET_4_HL(CPU* cpu);

void SET_5_A(CPU* cpu);
void SET_5_B(CPU* cpu);
void SET_5_C(CPU* cpu);
void SET_5_D(CPU* cpu);
void SET_5_E(CPU* cpu);
void SET_5_H(CPU* cpu);
void SET_5_L(CPU* cpu);
void SET_5_HL(CPU* cpu);

void SET_6_A(CPU* cpu);
void SET_6_B(CPU* cpu);
void SET_6_C(CPU* cpu);
void SET_6_D(CPU* cpu);
void SET_6_E(CPU* cpu);
void SET_6_H(CPU* cpu);
void SET_6_L(CPU* cpu);
void SET_6_HL(CPU* cpu);

void SET_7_A(CPU* cpu);
void SET_7_B(CPU* cpu);
void SET_7_C(CPU* cpu);
void SET_7_D(CPU* cpu);
void SET_7_E(CPU* cpu);
void SET_7_H(CPU* cpu);
void SET_7_L(CPU* cpu);
void SET_7_HL(CPU* cpu);

void RLC_B(CPU* cpu);
void RLC_C(CPU* cpu);
void RLC_D(CPU* cpu);
void RLC_E(CPU* cpu);
void RLC_H(CPU* cpu);
void RLC_L(CPU* cpu);
void RLC_HL(CPU* cpu);
void RLC_A(CPU* cpu);

void RRC_B(CPU* cpu);
void RRC_C(CPU* cpu);
void RRC_D(CPU* cpu);
void RRC_E(CPU* cpu);
void RRC_H(CPU* cpu);
void RRC_L(CPU* cpu);
void RRC_HL(CPU* cpu);
void RRC_A(CPU* cpu);

void SRA_A(CPU* cpu);
void SRA_B(CPU* cpu);
void SRA_C(CPU* cpu);
void SRA_D(CPU* cpu);
void SRA_E(CPU* cpu);
void SRA_H(CPU* cpu);
void SRA_L(CPU* cpu);
void SRA_HL(CPU* cpu);

const struct ExtendedOpcode extendedInstructions[256] =
{
	{ "RLC B", RLC_B },           // 0x00
	{ "RLC C", RLC_C },           // 0x01
	{ "RLC D", RLC_D },           // 0x02
	{ "RLC E", RLC_E },           // 0x03
	{ "RLC H", RLC_H },           // 0x04
	{ "RLC L", RLC_L },           // 0x05
	{ "RLC (HL)", RLC_HL },      // 0x06
	{ "RLC A", RLC_A },           // 0x07
	{ "RRC B", RRC_B },           // 0x08
	{ "RRC C", RRC_C },           // 0x09
	{ "RRC D", RRC_D },           // 0x0a
	{ "RRC E", RRC_E },           // 0x0b
	{ "RRC H", RRC_H },           // 0x0c
	{ "RRC L", RRC_L },           // 0x0d
	{ "RRC (HL)", RRC_HL },      // 0x0e
	{ "RRC A", RRC_A },           // 0x0f
	{ "RL B", RL_B },             // 0x10
	{ "RL C", RL_C },             // 0x11
	{ "RL D", RL_D },             // 0x12
	{ "RL E", RL_E },             // 0x13
	{ "RL H", RL_H },             // 0x14
	{ "RL L", RL_L },             // 0x15
	{ "RL (HL)", RL_HL },        // 0x16
	{ "RL A", RL_A },             // 0x17
	{ "RR B", RR_B },             // 0x18
	{ "RR C", RR_C },             // 0x19
	{ "RR D", RR_D },             // 0x1a
	{ "RR E", RR_E },             // 0x1b
	{ "RR H", RR_H },             // 0x1c
	{ "RR L", RR_L },             // 0x1d
	{ "RR (HL)", RR_HL },        // 0x1e
	{ "RR A", RR_A },             // 0x1f
	{ "SLA B", SLA_B },           // 0x20
	{ "SLA C", SLA_C },           // 0x21
	{ "SLA D", SLA_D },           // 0x22
	{ "SLA E", SLA_E },           // 0x23
	{ "SLA H", SLA_H },           // 0x24
	{ "SLA L", SLA_L },           // 0x25
	{ "SLA (HL)", SLA_HL },      // 0x26
	{ "SLA A", SLA_A },           // 0x27
	{ "SRA B", SRA_B },           // 0x28
	{ "SRA C", SRA_C },           // 0x29
	{ "SRA D", SRA_D },           // 0x2a
	{ "SRA E", SRA_E },           // 0x2b
	{ "SRA H", SRA_H },           // 0x2c
	{ "SRA L", SRA_L },           // 0x2d
	{ "SRA (HL)", SRA_HL },      // 0x2e
	{ "SRA A", SRA_A },           // 0x2f
	{ "SWAP B", SWAP_B },         // 0x30
	{ "SWAP C", SWAP_C },         // 0x31
	{ "SWAP D", SWAP_D },         // 0x32
	{ "SWAP E", SWAP_E },         // 0x33
	{ "SWAP H", SWAP_H },         // 0x34
	{ "SWAP L", SWAP_L },         // 0x35
	{ "SWAP (HL)", SWAP_HL },    // 0x36
	{ "SWAP A", SWAP_A },         // 0x37
	{ "SRL B", SRL_B },           // 0x38
	{ "SRL C", SRL_C },           // 0x39
	{ "SRL D", SRL_D },           // 0x3a
	{ "SRL E", SRL_E },           // 0x3b
	{ "SRL H", SRL_H },           // 0x3c
	{ "SRL L", SRL_L },           // 0x3d
	{ "SRL (HL)", SRL_HL },      // 0x3e
	{ "SRL A", SRL_A },           // 0x3f
	{ "BIT 0, B", BIT_0_B },      // 0x40
	{ "BIT 0, C", BIT_0_C },      // 0x41
	{ "BIT 0, D", BIT_0_D },      // 0x42
	{ "BIT 0, E", BIT_0_E },      // 0x43
	{ "BIT 0, H", BIT_0_H },      // 0x44
	{ "BIT 0, L", BIT_0_L },      // 0x45
	{ "BIT 0, (HL)", BIT_0_HL }, // 0x46
	{ "BIT 0, A", BIT_0_A },      // 0x47
	{ "BIT 1, B", BIT_1_B },      // 0x48
	{ "BIT 1, C", BIT_1_C },      // 0x49
	{ "BIT 1, D", BIT_1_D },      // 0x4a
	{ "BIT 1, E", BIT_1_E },      // 0x4b
	{ "BIT 1, H", BIT_1_H },      // 0x4c
	{ "BIT 1, L", BIT_1_L },      // 0x4d
	{ "BIT 1, (HL)", BIT_1_HL }, // 0x4e
	{ "BIT 1, A", BIT_1_A },      // 0x4f
	{ "BIT 2, B", BIT_2_B },      // 0x50
	{ "BIT 2, C", BIT_2_C },      // 0x51
	{ "BIT 2, D", BIT_2_D },      // 0x52
	{ "BIT 2, E", BIT_2_E },      // 0x53
	{ "BIT 2, H", BIT_2_H },      // 0x54
	{ "BIT 2, L", BIT_2_L },      // 0x55
	{ "BIT 2, (HL)", BIT_2_HL }, // 0x56
	{ "BIT 2, A", BIT_2_A },      // 0x57
	{ "BIT 3, B", BIT_3_B },      // 0x58
	{ "BIT 3, C", BIT_3_C },      // 0x59
	{ "BIT 3, D", BIT_3_D },      // 0x5a
	{ "BIT 3, E", BIT_3_E },      // 0x5b
	{ "BIT 3, H", BIT_3_H },      // 0x5c
	{ "BIT 3, L", BIT_3_L },      // 0x5d
	{ "BIT 3, (HL)", BIT_3_HL }, // 0x5e
	{ "BIT 3, A", BIT_3_A },      // 0x5f
	{ "BIT 4, B", BIT_4_B },      // 0x60
	{ "BIT 4, C", BIT_4_C },      // 0x61
	{ "BIT 4, D", BIT_4_D },      // 0x62
	{ "BIT 4, E", BIT_4_E },      // 0x63
	{ "BIT 4, H", BIT_4_H },      // 0x64
	{ "BIT 4, L", BIT_4_L },      // 0x65
	{ "BIT 4, (HL)", BIT_4_HL }, // 0x66
	{ "BIT 4, A", BIT_4_A },      // 0x67
	{ "BIT 5, B", BIT_5_B },      // 0x68
	{ "BIT 5, C", BIT_5_C },      // 0x69
	{ "BIT 5, D", BIT_5_D },      // 0x6a
	{ "BIT 5, E", BIT_5_E },      // 0x6b
	{ "BIT 5, H", BIT_5_H },      // 0x6c
	{ "BIT 5, L", BIT_5_L },      // 0x6d
	{ "BIT 5, (HL)", BIT_5_HL }, // 0x6e
	{ "BIT 5, A", BIT_5_A },      // 0x6f
	{ "BIT 6, B", BIT_6_B },      // 0x70
	{ "BIT 6, C", BIT_6_C },      // 0x71
	{ "BIT 6, D", BIT_6_D },      // 0x72
	{ "BIT 6, E", BIT_6_E },      // 0x73
	{ "BIT 6, H", BIT_6_H },      // 0x74
	{ "BIT 6, L", BIT_6_L },      // 0x75
	{ "BIT 6, (HL)", BIT_6_HL }, // 0x76
	{ "BIT 6, A", BIT_6_A },      // 0x77
	{ "BIT 7, B", BIT_7_B },      // 0x78
	{ "BIT 7, C", BIT_7_C },      // 0x79
	{ "BIT 7, D", BIT_7_D },      // 0x7a
	{ "BIT 7, E", BIT_7_E },      // 0x7b
	{ "BIT 7, H", BIT_7_H },      // 0x7c
	{ "BIT 7, L", BIT_7_L },      // 0x7d
	{ "BIT 7, (HL)", BIT_7_HL }, // 0x7e
	{ "BIT 7, A", BIT_7_A },      // 0x7f
	{ "RES 0, B", RES_0_B },      // 0x80
	{ "RES 0, C", RES_0_C },      // 0x81
	{ "RES 0, D", RES_0_D },      // 0x82
	{ "RES 0, E", RES_0_E },      // 0x83
	{ "RES 0, H", RES_0_H },      // 0x84
	{ "RES 0, L", RES_0_L },      // 0x85
	{ "RES 0, (HL)", RES_0_HL }, // 0x86
	{ "RES 0, A", RES_0_A },      // 0x87
	{ "RES 1, B", RES_1_B },      // 0x88
	{ "RES 1, C", RES_1_C },      // 0x89
	{ "RES 1, D", RES_1_D },      // 0x8a
	{ "RES 1, E", RES_1_E },      // 0x8b
	{ "RES 1, H", RES_1_H },      // 0x8c
	{ "RES 1, L", RES_1_L },      // 0x8d
	{ "RES 1, (HL)", RES_1_HL }, // 0x8e
	{ "RES 1, A", RES_1_A },      // 0x8f
	{ "RES 2, B", RES_2_B },      // 0x90
	{ "RES 2, C", RES_2_C },      // 0x91
	{ "RES 2, D", RES_2_D },      // 0x92
	{ "RES 2, E", RES_2_E },      // 0x93
	{ "RES 2, H", RES_2_H },      // 0x94
	{ "RES 2, L", RES_2_L },      // 0x95
	{ "RES 2, (HL)", RES_2_HL }, // 0x96
	{ "RES 2, A", RES_2_A },      // 0x97
	{ "RES 3, B", RES_3_B },      // 0x98
	{ "RES 3, C", RES_3_C },      // 0x99
	{ "RES 3, D", RES_3_D },      // 0x9a
	{ "RES 3, E", RES_3_E },      // 0x9b
	{ "RES 3, H", RES_3_H },      // 0x9c
	{ "RES 3, L", RES_3_L },      // 0x9d
	{ "RES 3, (HL)", RES_3_HL }, // 0x9e
	{ "RES 3, A", RES_3_A },      // 0x9f
	{ "RES 4, B", RES_4_B },      // 0xa0
	{ "RES 4, C", RES_4_C },      // 0xa1
	{ "RES 4, D", RES_4_D },      // 0xa2
	{ "RES 4, E", RES_4_E },      // 0xa3
	{ "RES 4, H", RES_4_H },      // 0xa4
	{ "RES 4, L", RES_4_L },      // 0xa5
	{ "RES 4, (HL)", RES_4_HL }, // 0xa6
	{ "RES 4, A", RES_4_A },      // 0xa7
	{ "RES 5, B", RES_5_B },      // 0xa8
	{ "RES 5, C", RES_5_C },      // 0xa9
	{ "RES 5, D", RES_5_D },      // 0xaa
	{ "RES 5, E", RES_5_E },      // 0xab
	{ "RES 5, H", RES_5_H },      // 0xac
	{ "RES 5, L", RES_5_L },      // 0xad
	{ "RES 5, (HL)", RES_5_HL }, // 0xae
	{ "RES 5, A", RES_5_A },      // 0xaf
	{ "RES 6, B", RES_6_B },      // 0xb0
	{ "RES 6, C", RES_6_C },      // 0xb1
	{ "RES 6, D", RES_6_D },      // 0xb2
	{ "RES 6, E", RES_6_E },      // 0xb3
	{ "RES 6, H", RES_6_H },      // 0xb4
	{ "RES 6, L", RES_6_L },      // 0xb5
	{ "RES 6, (HL)", RES_6_HL }, // 0xb6
	{ "RES 6, A", RES_6_A },      // 0xb7
	{ "RES 7, B", RES_7_B },      // 0xb8
	{ "RES 7, C", RES_7_C },      // 0xb9
	{ "RES 7, D", RES_7_D },      // 0xba
	{ "RES 7, E", RES_7_E },      // 0xbb
	{ "RES 7, H", RES_7_H },      // 0xbc
	{ "RES 7, L", RES_7_L },      // 0xbd
	{ "RES 7, (HL)", RES_7_HL }, // 0xbe
	{ "RES 7, A", RES_7_A },      // 0xbf
	{ "SET 0, B", SET_0_B },      // 0xc0
	{ "SET 0, C", SET_0_C },      // 0xc1
	{ "SET 0, D", SET_0_D },      // 0xc2
	{ "SET 0, E", SET_0_E },      // 0xc3
	{ "SET 0, H", SET_0_H },      // 0xc4
	{ "SET 0, L", SET_0_L },      // 0xc5
	{ "SET 0, (HL)", SET_0_HL }, // 0xc6
	{ "SET 0, A", SET_0_A },      // 0xc7
	{ "SET 1, B", SET_1_B },      // 0xc8
	{ "SET 1, C", SET_1_C },      // 0xc9
	{ "SET 1, D", SET_1_D },      // 0xca
	{ "SET 1, E", SET_1_E },      // 0xcb
	{ "SET 1, H", SET_1_H },      // 0xcc
	{ "SET 1, L", SET_1_L },      // 0xcd
	{ "SET 1, (HL)", SET_1_HL }, // 0xce
	{ "SET 1, A", SET_1_A },      // 0xcf
	{ "SET 2, B", SET_2_B },      // 0xd0
	{ "SET 2, C", SET_2_C },      // 0xd1
	{ "SET 2, D", SET_2_D },      // 0xd2
	{ "SET 2, E", SET_2_E },      // 0xd3
	{ "SET 2, H", SET_2_H },      // 0xd4
	{ "SET 2, L", SET_2_L },      // 0xd5
	{ "SET 2, (HL)", SET_2_HL }, // 0xd6
	{ "SET 2, A", SET_2_A },      // 0xd7
	{ "SET 3, B", SET_3_B },      // 0xd8
	{ "SET 3, C", SET_3_C },      // 0xd9
	{ "SET 3, D", SET_3_D },      // 0xda
	{ "SET 3, E", SET_3_E },      // 0xdb
	{ "SET 3, H", SET_3_H },      // 0xdc
	{ "SET 3, L", SET_3_L },      // 0xdd
	{ "SET 3, (HL)", SET_3_HL }, // 0xde
	{ "SET 3, A", SET_3_A },      // 0xdf
	{ "SET 4, B", SET_4_B },      // 0xe0
	{ "SET 4, C", SET_4_C },      // 0xe1
	{ "SET 4, D", SET_4_D },      // 0xe2
	{ "SET 4, E", SET_4_E },      // 0xe3
	{ "SET 4, H", SET_4_H },      // 0xe4
	{ "SET 4, L", SET_4_L },      // 0xe5
	{ "SET 4, (HL)", SET_4_HL }, // 0xe6
	{ "SET 4, A", SET_4_A },      // 0xe7
	{ "SET 5, B", SET_5_B },      // 0xe8
	{ "SET 5, C", SET_5_C },      // 0xe9
	{ "SET 5, D", SET_5_D },      // 0xea
	{ "SET 5, E", SET_5_E },      // 0xeb
	{ "SET 5, H", SET_5_H },      // 0xec
	{ "SET 5, L", SET_5_L },      // 0xed
	{ "SET 5, (HL)", SET_5_HL }, // 0xee
	{ "SET 5, A", SET_5_A },      // 0xef
	{ "SET 6, B", SET_6_B },      // 0xf0
	{ "SET 6, C", SET_6_C },      // 0xf1
	{ "SET 6, D", SET_6_D },      // 0xf2
	{ "SET 6, E", SET_6_E },      // 0xf3
	{ "SET 6, H", SET_6_H },      // 0xf4
	{ "SET 6, L", SET_6_L },      // 0xf5
	{ "SET 6, (HL)", SET_6_HL }, // 0xf6
	{ "SET 6, A", SET_6_A },      // 0xf7
	{ "SET 7, B", SET_7_B },      // 0xf8
	{ "SET 7, C", SET_7_C },      // 0xf9
	{ "SET 7, D", SET_7_D },      // 0xfa
	{ "SET 7, E", SET_7_E },      // 0xfb
	{ "SET 7, H", SET_7_H },      // 0xfc
	{ "SET 7, L", SET_7_L },      // 0xfd
	{ "SET 7, (HL)", SET_7_HL }, // 0xfe
	{ "SET 7, A", SET_7_A },      // 0xff
};

const unsigned char extendedInstructionTicks[256] =
{
	8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x0_
	8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x1_
	8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x2_
	8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x3_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x4_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x5_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x6_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x7_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x8_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x9_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xa_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xb_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xc_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xd_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xe_
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8  // 0xf_
};