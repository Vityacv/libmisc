
struct patchDef {
  patchDef *next;
  unsigned protect;
  unsigned char *adr;
  size_t sz;
};

class patchData {
  static patchData *object;
  patchData();
  patchData(const patchData &) = delete;
  patchData &operator=(const patchData &) = delete;
  patchDef *begin=0;
  void addPatch(unsigned char *adr, size_t sz, unsigned protect);
  unsigned char * addCodeSwapPatch(unsigned char *adr, unsigned char *dst, size_t sz, unsigned protect);
  patchDef * getPatch(unsigned char * adr);
  void freePatch(patchDef * pd);

 public:
  static patchData *instance();
  static void restorePatches();
  static void regcall restorePatch(unsigned char *adr);
  static void regcall addCode(unsigned char *adr, size_t sz = 100);
  static void regcall codeswap(unsigned char *src, unsigned char *dst, unsigned len);
};
