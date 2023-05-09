#define LDI 0
#define LDM 1
#define STR 2
#define ADD 3
#define SUB 4
#define JNZ 5
#define SWB 6
#define SWC 7
#define SWD 8
#define SWE 9
#define OR 10
#define AND 11
#define NOT 12
#define SHL 13
#define SHR 14
#define HLT 15

String opcodes[16] = {
  "ldi", "ldm", "str", "add", "sub", "jnz", "swb", "swc", "swd", "swe", "or", "and", "not", "shl", "shr", "hlt"
};

struct Registers {
  uint8_t a, b, c, d, e;
  uint16_t ip : 12;
} regs;

uint8_t memory[4096] = {
  LDI << 4, 0b00000011,
  SWD << 4, 0,
  LDI << 4, 5,
  SWB << 4, 0,
  ADD << 4, 0,
  SWE << 4, 0,
  SWD << 4, 0,
  SWB << 4, 0,
  LDI << 4, 1,
  SWC << 4, 0,
  SUB << 4, 0,
  SWD << 4, 0,
  SWE << 4, 0,
  SWC << 4, 0,
  LDI << 4, 4,
  JNZ << 4, 0,
  HLT << 4, 0,
};

uint16_t instruction = 0;
uint8_t hlt = 0;

inline void ldi() {
  regs.a = instruction & 0b11111111;
}

inline void ldm() {
  regs.a = memory[instruction & 0b11111111111];
}

inline void str() {
  memory[instruction & 0b11111111111] = regs.a;
}

inline void add() {
  regs.a = regs.b + regs.c;
}

inline void sub() {
  regs.a = regs.b - regs.c;
}

inline void jnz() {
  if (regs.d != 0) {
    regs.ip = regs.a + regs.f;
  }
}

inline void swb() {
  uint8_t tmp = regs.a;
  regs.a = regs.b;
  regs.b = tmp;
}

inline void swc() {
  uint8_t tmp = regs.a;
  regs.a = regs.c;
  regs.c = tmp;
}

inline void swd() {
  uint8_t tmp = regs.a;
  regs.a = regs.d;
  regs.d = tmp;
}

inline void swe() {
  uint8_t tmp = regs.a;
  regs.a = regs.e;
  regs.e = tmp;
}

inline void ior() {
  regs.a = regs.b | regs.c;
}

inline void iand() {
  regs.a = regs.b & regs.c;
}

inline void inot() {
  regs.a = ~regs.b;
}

inline void shl() {
  regs.a = regs.b << regs.c;
}

inline void shr() {
  regs.a = regs.b >> regs.c;
}

inline void fetch_instruction() {
  instruction = (memory[regs.ip] << 8) | (memory[regs.ip + 1]);
  regs.ip += 2;
}

inline void run_instruction() {
  switch (instruction >> 12) {
    case LDI:
      ldi();
      break;
    case LDM:
      ldm();
      break;
    case STR:
      str();
      break;
    case ADD:
      add();
      break;
    case SUB:
      sub();
      break;
    case JNZ:
      jnz();
      break;
    case SWB:
      swb();
      break;
    case SWC:
      swc();
      break;
    case SWD:
      swd();
      break;
    case SWE:
      swe();
      break;
    case OR:
      ior();
      break;
    case AND:
      iand();
      break;
    case NOT:
      inot();
      break;
    case SHL:
      shl();
      break;
    case SHR:
      shr();
      break;
    case HLT:
      hlt = 1;
      break;
    default:
      break;
  }
}

void print(String str) {
  char * buffer = (char *) malloc(str.length() + 1);
  sprintf(buffer, str.c_str());
  Serial.print(buffer);
}

void print_status() {
  char buffer[100];
  sprintf(buffer, "\na: 0x%02x\tb: 0x%02x\tc: 0x%02x\td: 0x%02x\te: 0x%02x\tip: 0x%03x\topcode: %s\tvalue: 0x%03x", regs.a, regs.b, regs.c, regs.d, regs.e, regs.f, regs.ip, opcodes[instruction >> 12].c_str(), instruction & 0b111111111111);
  Serial.print(buffer);
}

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(250000);
  print("\n\n\n\n\n\n\n\n\n\n\n\nBOOT:\n\n\n");
  print_status();
}

void loop() {
  fetch_instruction();
  run_instruction();
  print_status();

  if (hlt) {
    print("\n\nHALT!\n\n");

    while (1);
  }

  delay(200);
}