#include "hookapi.h"

#define COPY32(src, dst)                                                       \
  {                                                                            \
    uint64_t *x = (dst);                                                       \
    uint64_t *y = (src);                                                       \
    *x++ = *y++;                                                               \
    *x++ = *y++;                                                               \
    *x++ = *y++;                                                               \
    *x++ = *y++;                                                               \
  }

typedef struct {
  uint16_t index;
  uint8_t target[32];
} REGIST_VALUE;

typedef struct {
  uint8_t target[32];
  uint16_t count;
} VOTE_VALUE;

int64_t hook(uint32_t reserved) {
  _g(1, 1);

  if (otxn_type() != ttINVOKE)
    DONEMSG("not invoke");

  uint8_t hook_acc[20];
  hook_account(SBUF(hook_acc));

  uint8_t otxn_acc[20];
  otxn_field(SBUF(otxn_acc), sfAccount);

  if (BUFFER_EQUAL_20(hook_acc, otxn_acc)) {
    // regist new vote target
    uint8_t registered_count = 0;
    for (uint8_t i = 0; GUARD(16), i < 16; i++) {
      REGIST_VALUE regist_value = {};
      int64_t len = otxn_param(SVAR(regist_value), SVAR(i));
      if (len == DOESNT_EXIST) {
        continue;
      }
      if (len != sizeof(REGIST_VALUE)) {
        rollback(SBUF("Invalid vote target"), len);
      }
      VOTE_VALUE vote_value = {};
      vote_value.count = 0;
      COPY32(regist_value.target, vote_value.target);
      ASSERT(state_set(SVAR(vote_value), SVAR(regist_value.index)) > 0);
      registered_count++;
    }
    if (registered_count == 0) {
      rollback(SBUF("No vote target"), __LINE__);
    }
    DONEMSG("Candidate registered");
  }

  uint16_t target;
  if (otxn_param(SVAR(target), "KEY", 3) < 0) {
    rollback(SBUF("Invalid vote target KEY"), __LINE__);
  }

  VOTE_VALUE vote_value;
  if (state(SVAR(vote_value), SVAR(target)) < 0) {
    rollback(SBUF("Vote target not found"), __LINE__);
  }

  uint8_t voted_target[32];
  if (state(SVAR(voted_target), SBUF(otxn_acc)) >= 0) {
    rollback(SBUF("Already voted"), __LINE__);
  }

  // increment vote count
  vote_value.count++;
  ASSERT(state_set(SVAR(vote_value), SVAR(target)) > 0);

  // save account as voted
  ASSERT(state_set(SBUF(vote_value.target), SBUF(otxn_acc)) > 0);

  DONEMSG("Vote recorded successfully");

  return 0;
}
