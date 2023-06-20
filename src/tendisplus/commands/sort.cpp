// Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
// Please refer to the license text that comes with this tendis open source
// project for additional information.

#include <string>
#include <vector>
#include <algorithm>
#ifndef _WIN32
#include <experimental/optional>
#endif
#include "tendisplus/storage/varint.h"
#include "tendisplus/commands/command.h"
#include "tendisplus/utils/string.h"
#include "tendisplus/utils/invariant.h"
#include "tendisplus/storage/skiplist.h"

namespace tendisplus {

class OodbCommand : public Command {
 public:
  TimeCommand() : Command("ooquery", "RF") {}

  ssize_t arity() const {
    return 1;
  }

  int32_t firstkey() const {
    return 0;
  }

  int32_t lastkey() const {
    return 0;
  }

  int32_t keystep() const {
    return 0;
  }

  

  std::vector<int> getKeysFromCommand(const std::vector<std::string>& argv) {
    uint32_t i, j;
    std::vector<int> keys;

    keys.push_back(1); /* <sort-key> is always present. */

    /* Search for STORE option. By default we consider options to don't
     * have arguments, so if we find an unknown option name we scan the
     * next. However there are options with 1 or 2 arguments, so we
     * provide a list here in order to skip the right number of args. */
    struct {
      std::string name;
      int skip;
    } skiplist[] = {
      {"limit", 2}, {"get", 1}, {"by", 1}, {"", 0} /* End of elements. */
    };

    for (i = 2; i < argv.size(); i++) {
      for (j = 0; skiplist[j].name != ""; j++) {
        if (toLower(argv[i]) == skiplist[j].name) {
          i += skiplist[j].skip;
          break;
        } else if (toLower(argv[i]) == "store" && i + 1 < argv.size()) {
          keys.push_back(i + 1); /* <store-key> */
          break;
        }
      }
    }
    return keys;
  }

  Expected<std::string> run(Session* sess) final {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::stringstream ss;
    Command::fmtMultiBulkLen(ss, 2);
    Command::fmtBulk(ss, std::to_string(tv.tv_sec));
    Command::fmtBulk(ss, std::to_string(tv.tv_usec));
    
    
    auto server = sess->getServerEntry();
    auto pCtx = sess->getCtx();
    auto keyidx = getKeysFromCommand(sess->getArgs());
    auto expdblist = server->getSegmentMgr()->getAllKeysLocked(
      sess,
      sess->getArgs(),
      keyidx,
      store ? mgl::LockMode::LOCK_X : Command::RdLock());
    RET_IF_ERR_EXPECTED(expdblist);

    auto expdb = server->getSegmentMgr()->getDbHasLocked(sess, key);
    RET_IF_ERR_EXPECTED(expdb);
    PStore kvstore = expdb.value().store;

    /* 3. Get the sort key and length */
    auto expRv =
      Command::expireKeyIfNeeded(sess, key, RecordType::RT_DATA_META);
    if (expRv.status().code() == ErrorCodes::ERR_EXPIRED ||
        expRv.status().code() == ErrorCodes::ERR_NOTFOUND) {
      exist = false;
    } else if (!expRv.ok()) {
      return expRv.status();
    }

    
    return ss.str();
  }
} oodbCmd;


//class SortCommand : public Command {
 
}  // namespace tendisplus
