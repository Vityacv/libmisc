#include "pch.h"
#include "stl.h"
#include "memory.h"
#include "patch.h"

patchData *patchData::object = 0;

patchData::patchData() {}

patchData *patchData::instance() {
  if (object == 0) {
    patchData *obj = new patchData();
    object = obj;
  }

  return object;
}

void regcall patchData::addCode(unsigned char *adr, size_t sz) {
  patchData *pData = patchData::instance();
  pData->addPatch(adr, sz, PAGE_EXECUTE_READWRITE);
};

void regcall patchData::codeswap(unsigned char *src, const unsigned char *dst, unsigned len) {
	patchData *pData = patchData::instance();
	memswap(src,pData->addCodeSwapPatch(src,dst,len,PAGE_EXECUTE_READWRITE),len);
}


unsigned char * patchData::addCodeSwapPatch(unsigned char *adr, const unsigned char *dst, size_t sz, unsigned protect) {
  patchDef * pd = getPatch(adr);
  if (adr && !pd) {
    pd = (patchDef *)malloc(sizeof(patchDef) + sz*2);
    if (!pd) return 0;
    VirtualProtect(adr, sz, protect, (PDWORD)&pd->protect);
    memcpy((unsigned char *)pd + sizeof(patchDef), adr, sz);
    memcpy((unsigned char *)pd + sizeof(patchDef) + sz, dst, sz);

    pd->adr = adr;
    pd->sz = sz;
    pd->next = begin;
    begin = pd;
  }
  return (unsigned char *)((unsigned char *)pd + sizeof(patchDef) + sz);
}

void patchData::addPatch(unsigned char *adr, size_t sz, unsigned protect) {
  if (adr && !getPatch(adr)) {
    patchDef *pd = (patchDef *)malloc(sizeof(patchDef) + sz);
    if (!pd) return;
    VirtualProtect(adr, sz, protect, (PDWORD)&pd->protect);
    memcpy((unsigned char *)pd + sizeof(patchDef), adr, sz);
    pd->adr = adr;
    pd->sz = sz;
    pd->next = begin;
    begin = pd;
  }
}

void patchData::freePatch(patchDef * pd){
      unsigned tmp;
      MEMORY_BASIC_INFORMATION minfo;
      VirtualQuery(pd->adr, &minfo, sizeof(MEMORY_BASIC_INFORMATION));
      if(minfo.State != MEM_FREE){
        VirtualProtect(pd->adr, pd->sz, PAGE_EXECUTE_READWRITE, (PDWORD)&tmp);
        memcpy(pd->adr, (unsigned char *)pd + sizeof(patchDef), pd->sz);
        VirtualProtect(pd->adr, pd->sz, pd->protect, (PDWORD)&tmp);
      }
      free(pd);
}

void regcall patchData::restorePatch(unsigned char *adr) {
  patchData *pData = patchData::instance();
  patchDef * pd = pData->begin;
  patchDef * prevpd = 0;
  while (pd) {
    if (pd->adr == adr) {
      if (prevpd)
        prevpd->next = pd->next;
      else
        pData->begin = pd->next;
      pData->freePatch(pd);
      return;
    }
    prevpd = pd;
    pd = pd->next;
  }
}

void patchData::restorePatches() {
  patchData *pData = patchData::instance();
  patchDef *pd = pData->begin;
  if (pd) {
    while (pd) {
      patchDef *temp = pd->next;
      pData->freePatch(pd);
      pd = temp;
    }
  }
}

patchDef *patchData::getPatch(unsigned char *adr) {
  patchDef *pd = begin;
  if (pd) {
    while (pd) {
      patchDef *temp = pd->next;
      if (pd->adr == adr) return pd;
      pd = temp;
    }
  }
  return nullptr;
}
