#include "PolyReader.h"
#include <algorithm>

XPolynomial *PolyReader::ReadXPolynomial(char *stream) {
  int start = 0, end = 0;
  while (stream[end++])
    ;

  return _ReadXPolynomial(stream, start, end - 1);
}

//
// XPolynomial is a comma separated list of xterms
// {XT1, XT2, ...}
//
XPolynomial *PolyReader::_ReadXPolynomial(char *stream, int start, int end) {
  XPolynomial *xpol = new XPolynomial();

  int s1, e1 = start;

  do {
    s1 = _GotoOpenBracket(stream, e1 + 1, end);
    if (s1 >= 0) {
      e1 = _GotoCloseBracket(stream, s1 + 1, end);
      _Assert(e1 >= 0, "Right bracket missing!");

      // create XTerm and add it to the xpol
      XTerm *xt = _ReadXTerm(stream, s1, e1);
      xpol->AddTerm(xt);
      xt->Dispose();
    }
  } while (s1 > 0);

  return xpol;
}

//
// XTerm is structure that begins with UFraction
// (given by two UPolynomials) and list of Powers
//
// {{UP1, UP2}, XM1, XM2, ...} : type1
//
// Alternatively, it could start with a real coefficient
// {C, XM1, XM2, ...} : type2
//
XTerm *PolyReader::_ReadXTerm(char *stream, int s, int e) {
  XTerm *xt = new XTerm();

#if DESER_DBG
  // debug
  Log::PrintLogF(0, "xt: ");
  _Print(stream, s, e);
#endif

  int s1, e1 = s;

  // which type of xterm
  char c = _GotoNextChar(stream, e1 + 1);

  if (c == '{') {
    // type 1

    // read UFraction
    s1 = _GotoOpenBracket(stream, e1 + 1, e);
    _Assert(s1 >= 0, "Left bracket missing!");

    e1 = _GotoCloseBracket(stream, s1 + 1, e);
    _Assert(e1 >= 0, "Right bracket missing!");

    UPolynomialFraction *uf = _ReadUFraction(stream, s1, e1);
    xt->SetUFraction(uf);
    uf->Dispose();
  } else {
    // type 2

    // read real coeff
    REAL cf;
    sscanf(&stream[e1 + 1], "%lf", &cf);

    UPolynomialFraction *uf = new UPolynomialFraction(cf);
    xt->SetUFraction(uf);
    uf->Dispose();
  }

  // read xmonoms
  do {
    s1 = _GotoOpenBracket(stream, e1 + 1, e);
    if (s1 >= 0) {
      e1 = _GotoCloseBracket(stream, s1 + 1, e);
      _Assert(e1 >= 0, "Right bracket missing!");

      // create Power and add it to the xpol
      Power *xp = _ReadXPower(stream, s1, e1);
      xt->AddPower(xp);
      xp->Dispose();
    }
  } while (s1 > 0);

  return xt;
}

//
// UPolynomialFraction is a pair of two UPolynomials
// the first is numerator and the second is denominator
// {N, D}
//
UPolynomialFraction *PolyReader::_ReadUFraction(char *stream, int s, int e) {
#if DESER_DBG
  // debug
  Log::PrintLogF(0, "uf: ");
  _Print(stream, s, e);
#endif

  // read numerator
  int s1, e1 = s;
  s1 = _GotoOpenBracket(stream, e1 + 1, e);
  _Assert(s1 >= 0, "Left bracket missing!");

  e1 = _GotoCloseBracket(stream, s1 + 1, e);
  _Assert(e1 >= 0, "Right bracket missing!");

  UPolynomial *up1 = _ReadUPolynomial(stream, s1, e1);

  // read denominator
  s1 = _GotoOpenBracket(stream, e1 + 1, e);
  _Assert(s1 >= 0, "Left bracket missing!");

  e1 = _GotoCloseBracket(stream, s1 + 1, e);
  _Assert(e1 >= 0, "Right bracket missing!");

  UPolynomial *up2 = _ReadUPolynomial(stream, s1, e1);

  // create UFraction
  UPolynomialFraction *uf = new UPolynomialFraction();

  uf->SetNumerator(up1);
  up1->Dispose();

  uf->SetDenominator(up2);
  up2->Dispose();

  return uf;
}

//
// UPolynomial is a list of UTerms
//
UPolynomial *PolyReader::_ReadUPolynomial(char *stream, int s, int e) {
#if DESER_DBG
  // debug
  Log::PrintLogF(0, "up: ");
  _Print(stream, s, e);
#endif

  UPolynomial *upol = new UPolynomial();

  int s1, e1 = s;

  do {
    s1 = _GotoOpenBracket(stream, e1 + 1, e);
    if (s1 >= 0) {
      e1 = _GotoCloseBracket(stream, s1 + 1, e);
      _Assert(e1 >= 0, "Right bracket missing!");

      // create UTerm and add it to the xpol
      UTerm *ut = _ReadUTerm(stream, s1, e1);
      upol->AddTerm(ut);
      ut->Dispose();
    }
  } while (s1 > 0);

  return upol;
}

//
// UTerm is a structure where first item is a real coefficient
// and then there is a list of Powers
//
UTerm *PolyReader::_ReadUTerm(char *stream, int s, int e) {
#if DESER_DBG
  // debug
  Log::PrintLogF(0, "ut: ");
  _Print(stream, s, e);
#endif

#ifdef UTERMS_BANK
  UTerm *ut = UTermsBank::UTermsFactory.AcquireUTerm();
#else
  UTerm *ut = new UTerm();
#endif

  // read real coefficient
  int s1 = s + 1, e1;
  e1 = _GotoSeparator(stream, s + 1, e);
  _Assert(e1 >= 0, "Separator missing!");

  REAL cf;
  sscanf(&stream[s1], "%lf", &cf);

#if DESER_DBG
  // debug
  Log::PrintLogF(0, "  cf = %lf\n", cf);
#endif
  ut->SetCoeff(cf);

  // read upowers
  do {
    s1 = _GotoOpenBracket(stream, e1 + 1, e);
    if (s1 >= 0) {
      e1 = _GotoCloseBracket(stream, s1 + 1, e);
      _Assert(e1 >= 0, "Right bracket missing!");

      // create UTerm and add it to the xpol
      Power *up = _ReadUPower(stream, s1, e1);
      ut->AddPower(up);
      up->Dispose();
    }
  } while (s1 > 0);

  return ut;
}

//
// Power is a pair of index and degree
// {1, 2}
//
Power *PolyReader::_ReadUPower(char *stream, int s, int /*e*/) {
#if DESER_DBG
  // debug
  Log::PrintLogF(0, "uw: ");
  _Print(stream, s, e);
#endif

  int index, degree;
  int c = sscanf(&stream[s], "{%d,%d}", &index, &degree);
  _Assert(c == 2, "Failed to load upower!");

#if DESER_DBG
  // debug
  Log::PrintLogF(0, "  index = %d, degree = %d\n", index, degree);
#endif

#ifdef POWER_BANK
  Power *up = PowerBank::PowerFactory.AcquirePower(index, degree, VAR_TYPE_U);
#else
  Power *up = new Power(index, degree, VAR_TYPE_U);
#endif

  return up;
}

//
// Power is a pair of index and degree
// {1, 2}
//
Power *PolyReader::_ReadXPower(char *stream, int s, int /*e*/) {
#if DESER_DBG
  // debug
  Log::PrintLogF(0, "xw: ");
  _Print(stream, s, e);
#endif

  int index, degree;
  int c = sscanf(&stream[s], "{%d,%d}", &index, &degree);
  _Assert(c == 2, "Failed to load xpower!");

#if DESER_DBG
  // debug
  Log::PrintLogF(0, "  index = %d, degree = %d\n", index, degree);
#endif

#ifdef POWER_BANK
  Power *xp = PowerBank::PowerFactory.AcquirePower(index, degree, VAR_TYPE_X);
#else
  Power *xp = new Power(index, degree, VAR_TYPE_X);
#endif

  return xp;
}

void PolyReader::_Print(char *stream, int s, int e) {
  for (int ii = s; ii <= e; ii++) {
    Log::PrintLogF(0, "%c", stream[ii]);
  }
  Log::PrintLogF(0, "\n");
}

void PolyReader::_Error(const char *msg) {
  Log::PrintLogF(0, msg);
  throw - 1;
}

void PolyReader::_Assert(bool assert, const char *msg) {
  if (!assert) {
    _Error(msg);
  }
}

int PolyReader::_GotoOpenBracket(char *stream, int pos, int end) {
  while (pos <= end && stream[pos] != '{') {
    pos++;
  }

  if (pos > end) {
    return -1;
  }
  return pos;
}

int PolyReader::_GotoSeparator(char *stream, int pos, int end) {
  while (pos <= end && stream[pos] != ',' && stream[pos] != '}') {
    pos++;
  }

  if (pos > end) {
    return -1;
  }
  return pos;
}

int PolyReader::_GotoCloseBracket(char *stream, int s, int end) {
  int diff = 1;
  while (s <= end && diff != 0) {
    if (stream[s] == '{') {
      diff++;
    }
    if (stream[s] == '}') {
      diff--;
    }
    s++;
  }

  if (s > end) {
    return -1;
  }

  return s - 1;
}

char PolyReader::_GotoNextChar(char *stream, int s) {
  while (stream[s] == ' ' || stream[s] == '\t' || stream[s] == '\n') {
    s++;
  }
  return stream[s];
}

void PolyReader::PrintPolynomials(std::vector<XPolynomial *> &vpols, int level,
                                  int size) {
  Log::PrintLogF(1, "\\begin{eqnarray*}\n");
  Log::PrintLogF(2, "<polynomial_system>\n");

  unsigned count = size > 0 ? std::min((size_t)size, vpols.size()) : vpols.size();
  for (unsigned i = 0; i < count; i++) {
    PrintPolynomial(vpols[i], level, i, true);
  }

  Log::PrintLogF(1, "\\end{eqnarray*}\n");
  Log::PrintLogF(2, "</polynomial_system>\n");
}

void PolyReader::PrintPolynomial(XPolynomial *xp, int level, int index,
                                 bool tabular) {
  if (Log::LogEnabledForLevel(level) == false) {
    return;
  }

  // don't print too big polynomials
  if (xp->GetTotalTermCount() > 255) {
    // too big
    if (tabular)
      Log::PrintLogF(1, " & & \\mbox\{");
    Log::OutputText(" Polynomial too big for output (number of terms is %d)\n",
                    xp->GetTermCount());
    if (tabular)
      Log::PrintLogF(1, "} \\\\ \n");
    Log::PrintLogF(2, "\n\n");
    return;
  }

  char *ltx = xp->PrintLatex();
  static int cnt = 0;

  // separate chunks
  const int chunk = 122;
  const int chunkDelta = 20;
  int curChunk = chunk;
  int size = 0;
  while (ltx[size++])
    ;
  size--; // added on 10.2015

  // failed to print too large polynomials
  if (size > 1000) {
    if (!tabular) {
      Log::OutputText("\nPolynomial too big for output (text size is %d "
                      "characters, number of terms is %d)",
                      size, xp->GetTermCount());
      Log::PrintLogF(1, "\\\\ \n\n");
    } else {
      if (level == 1) {
        Log::PrintLogF(2, "<polynomial>p_{%d} = ...</polynomial>\n", index);
      }
      if (tabular && index > 0) {
        Log::PrintLogF(1, "p_{%d} &=& ", index);
      }
      Log::PrintLogF(1, "\\ldots \\\\ \n ");
      Log::PrintLogF(2, "\n\n");
    }
    delete[] ltx;
    return;
  }

  if (level == 1) {
    Log::PrintLogF(2, "<polynomial>\n");
  }

  int pos = 0, posb = 0;

  bool first = true;
  while (pos < size) {
    posb = pos;
    pos += curChunk;
    if (pos > size) {
      pos = size;
    }

    // must break on a correct place
    // combination +x, -x, +(, -(
    while (pos > 1 /* added on 10.2015 */ && pos < size && pos > posb &&
           ((ltx[pos - 1] != 'x' && ltx[pos - 1] != '(') ||
            (ltx[pos - 2] != '+' && ltx[pos - 2] != '-'))) {
      pos--;
    }
    if (pos > 1 /* added on 10.2015 */ && pos < size) {
      pos--;
    }

    if (pos <= posb) {
      // could stop on + or - sign if and only if
      // there is equal number of characters '{' and '}' after
      pos += curChunk;
      if (pos > size) {
        pos = size;
      }

      bool found = false;
      while (found == false) {
        if (pos == posb) {
          found = true;
        } else {
          if (ltx[pos - 1] == '+' || ltx[pos - 1] == '-') {
            // check '{' and '}' after
            int diff = 0;
            for (int bi = pos; bi < size; bi++) {
              if (ltx[bi] == '{') {
                diff++;
              } else if (ltx[bi] == '}') {
                diff--;
              }
            }

            found = (diff == 0);
          }
        }
        if (found == false) {
          pos--;
        }
      }
    }

    if (pos <= posb) {
      // impossible to break with current chunk
      // try with greater chunk
      curChunk += chunkDelta;
      pos = posb;
      continue;
    } else {
      curChunk = chunk;
    }

    if (!tabular) {
      Log::PrintLogF(level, "$$\n");
    }
    if (first) {
      first = false;
      ++cnt;
      if (index == -2) {
        // conclusion
        Log::PrintLogF(level, "g %c=%c ", tabular ? '&' : ' ',
                       tabular ? '&' : ' ');
        if (level == 1) {
          Log::PrintLogF(2, "g = ");
        }
      } else {
        Log::PrintLogF(level, "p_{%d} %c=%c ", index >= 0 ? index : cnt,
                       tabular ? '&' : ' ', tabular ? '&' : ' ');
        if (level == 1) {
          Log::PrintLogF(2, "p_{%d} = ", index >= 0 ? index : cnt);
        }
      }
    } else if (tabular) {
      Log::PrintLogF(level, "& & ");
    }

    char ct = ltx[pos];
    ltx[pos] = 0;

    Log::PrintLogF(level, "%s", &ltx[posb]);
    if (level == 1) {
      Log::PrintLogF(2, "%s", &ltx[posb]);
    }
    if (tabular) {
      Log::PrintLogF(level, "\\\\");
    }
    Log::PrintLogF(level, "\n");
    ltx[pos] = ct;

    if (!tabular) {
      Log::PrintLogF(level, "$$\n");
    }
  }
  if (level == 1) {
    Log::PrintLogF(2, "</polynomial>\n");
  }

  delete[] ltx;
}
