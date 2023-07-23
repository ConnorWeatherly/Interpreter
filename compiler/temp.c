static bool
_mlt(ASTNode* node, ErrorReport* report) {
  int64_t intNum;
  double  dblNum;

  switch (node->left.dataType) {
    case INTGR32: {
      switch (node->right.dataType) {
        case INTGR32: {
          // Check for overflow into 64 bits
          intNum = node->left.value.asInt32*node->right.value.asInt32;
          if (intNum >= INT32_MIN && intNum <= INT32_MAX) {
            node->left.value.asInt32  = intNum;
          } else {
            node->left.dataType       = INTGR64;
            node->left.value.asInt64  = intNum;
          }
          return true;
        }
        case INTGR64: {
          // Check for overflow past 64 bits
          uint32_t absLeft  = _abs_int32(node->left.value.asInt32);
          uint64_t absRight = _abs_int64(node->right.value.asInt64);
          if ((absLeft != 0) && (INT64_MAX/absLeft < absRight)) {
            *report = ERR_OPP_INT_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          intNum = node->left.value.asInt32*node->right.value.asInt64;
          if (intNum >= INT32_MIN && intNum <= INT32_MAX) {
            node->left.value.asInt32  = intNum;
          } else {
            node->left.dataType       = INTGR64;
            node->left.value.asInt64  = intNum;
          }
          return true;
        }
        case FLT: {
          // Check for size of new float
          dblNum = node->left.value.asInt32*node->right.value.asFloat;
          // if (dblNum <= FLT_MAX && dblNum >= FLT_MIN) {
          //   node->left.dataType       = FLT;
          //   node->left.value.asFloat  = dblNum;
          // } else {
          //   node->left.dataType       = DBL;
          //   node->left.value.asDouble = dblNum;
          // }

          node->left.dataType       = DBL;
          node->left.value.asDouble = dblNum;
          return true;
        }
        case DBL: {
          // Check for overflow past double
          uint32_t  absLeft  = _abs_int32(node->left.value.asInt32);
          double    absRight = _abs_dbl(node->right.value.asDouble);
          if ((absLeft != 0) && (DBL_MAX/absLeft < absRight)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          dblNum = node->left.value.asInt32*node->right.value.asDouble;

          node->left.dataType       = DBL;
          node->left.value.asDouble = dblNum;
          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    case INTGR64: {
      switch (node->right.dataType) {
        case INTGR32: {
          // Check for overflow into 64 bits
          uint64_t absLeft  = _abs_int64(node->left.value.asInt64);
          uint32_t absRight = _abs_int32(node->right.value.asInt32);
          if ((absLeft != 0) && (INT64_MAX/absLeft < absRight)) {
            *report = ERR_OPP_INT_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          intNum = node->left.value.asInt64*node->right.value.asInt32;
          if (intNum >= INT32_MIN && intNum <= INT32_MAX) {
            node->left.dataType       = INTGR32;
            node->left.value.asInt32  = intNum;
          } else {
            node->left.value.asInt64  = intNum;
          }
          return true;
        }
        case INTGR64: {
          // Check for overflow past 64 bits
          uint64_t absLeft  = _abs_int64(node->left.value.asInt64);
          uint64_t absRight = _abs_int64(node->right.value.asInt64);
          if ((absLeft != 0) && (INT64_MAX/absLeft < absRight)) {
            *report = ERR_OPP_INT_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          intNum = node->left.value.asInt64*node->right.value.asInt64;
          if (intNum >= INT32_MIN && intNum <= INT32_MAX) {
            node->left.dataType       = INTGR32;
            node->left.value.asInt32  = intNum;
          } else {
            node->left.value.asInt64  = intNum;
          }
          return true;
        }
        case FLT: {
          // Check for size of new float
          dblNum = node->left.value.asInt64*node->right.value.asFloat;
          // if (dblNum <= FLT_MAX && dblNum >= FLT_MIN) {
          //   node->left.dataType       = FLT;
          //   node->left.value.asFloat  = dblNum;
          // } else {
          //   node->left.dataType       = DBL;
          //   node->left.value.asDouble = dblNum;
          // }

          node->left.dataType       = DBL;
          node->left.value.asDouble = dblNum;
          return true;
        }
        case DBL: {
          // Check for overflow past double
          uint64_t  absLeft   = _abs_int64(node->left.value.asInt64);
          double    absRight  = _abs_dbl(node->right.value.asDouble);
          if ((absLeft != 0) && (DBL_MAX/absLeft < absRight)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          dblNum = node->left.value.asInt64*node->right.value.asDouble;

          node->left.dataType       = DBL;
          node->left.value.asDouble = dblNum;
          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    case FLT: {
      switch (node->right.dataType) {
        case INTGR32: {
          // Check for size of new float
          dblNum = node->left.value.asFloat*node->right.value.asInt32;
          // if (dblNum <= FLT_MAX && dblNum >= FLT_MIN) {
          //   node->left.value.asFloat  = dblNum;
          // } else {
          //   node->left.dataType       = DBL;
          //   node->left.value.asDouble = dblNum;
          // }

          node->left.dataType       = DBL;
          node->left.value.asDouble = dblNum;
          return true;
        }
        case INTGR64: {
          // Check for size of new float
          dblNum = node->left.value.asFloat*node->right.value.asInt64;
          // if (dblNum <= FLT_MAX && dblNum >= FLT_MIN) {
          //   node->left.value.asFloat  = dblNum;
          // } else {
          //   node->left.dataType       = DBL;
          //   node->left.value.asDouble = dblNum;
          // }

          node->left.dataType       = DBL;
          node->left.value.asDouble = dblNum;
          return true;
        }
        case FLT: {
          // Check for size of new float
          dblNum = node->left.value.asFloat*node->right.value.asFloat;
          // if (dblNum <= FLT_MAX && dblNum >= FLT_MIN) {
          //   node->left.value.asFloat  = dblNum;
          // } else {
          //   node->left.dataType       = DBL;
          //   node->left.value.asDouble = dblNum;
          // }

          node->left.dataType       = DBL;
          node->left.value.asDouble = dblNum;
          return true;
        }
        case DBL: {
          // Check for overflow past double
          float   absLeft   = _abs_flt(node->left.value.asFloat);
          double  absRight  = _abs_dbl(node->right.value.asDouble);
          if (_flt_not_decimal(absLeft) &&
          (absLeft != 0) && (DBL_MAX/absLeft < absRight)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // if (dblNum <= FLT_MAX && dblNum >= FLT_MIN) {
          //   node->left.value.asFloat  = dblNum;
          // } else {
          //   node->left.dataType       = DBL;
          //   node->left.value.asDouble = dblNum;
          // }

          dblNum = node->left.value.asFloat*node->right.value.asDouble;

          node->left.dataType       = DBL;
          node->left.value.asDouble = dblNum;
          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    case DBL: {
      switch (node->right.dataType) {
        case INTGR32: {
          // Check for overflow past double
          double    absLeft   = _abs_dbl(node->left.value.asDouble);
          uint32_t  absRight  = _abs_int32(node->right.value.asInt32);
          if ((absRight != 0) && (DBL_MAX/absRight < absLeft)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          dblNum = node->left.value.asDouble*node->right.value.asInt32;

          node->left.value.asDouble = dblNum;
          return true;
        }
        case INTGR64: {
          // Check for overflow past double
          double    absLeft   = _abs_dbl(node->left.value.asDouble);
          uint64_t  absRight  = _abs_int64(node->right.value.asInt64);
          if ((absRight != 0) && (DBL_MAX/absRight < absLeft)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          dblNum = node->left.value.asDouble*node->right.value.asInt64;

          node->left.value.asDouble = dblNum;
          return true;
        }
        case FLT: {
          // Check for overflow past double
          double  absLeft   = _abs_dbl(node->left.value.asDouble);
          float   absRight  = _abs_flt(node->right.value.asFloat);
          if (_dbl_not_decimal(absLeft) &&
          (absLeft != 0) && (DBL_MAX/absLeft < absRight)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          dblNum = node->left.value.asDouble*node->right.value.asFloat;

          node->left.value.asDouble = dblNum;
          return true;
        }
        case DBL:{
          // Check for overflow past double
          double absLeft  = _abs_dbl(node->left.value.asDouble);
          double absRight = _abs_dbl(node->right.value.asDouble);
          if (_dbl_not_decimal(absLeft) &&
          (absLeft != 0) && (DBL_MAX/absLeft < absRight)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          dblNum = node->left.value.asDouble*node->right.value.asDouble;

          node->left.value.asDouble = dblNum;
          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  return false;
}
