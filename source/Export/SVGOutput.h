// SVGOutput.h: interface for the CSVGOutput class.
//
//////////////////////////////////////////////////////////////////////

#include <ostream>
#if !defined(SVGOUTPUT_H)
#define SVGOUTPUT_H

#include "GCLCOutput.h"

class CSVGOutput : public CGCLCOutput {
public:
  CSVGOutput(std::ostream &h);
  virtual ~CSVGOutput();

  void Reset();
  GReturnValue Init(int x, int y);
  GReturnValue Close();
  GReturnValue SetPixel(double x, double y);
  GReturnValue DrawSegment(double x1, double y1, double x2, double y2);
  GReturnValue DrawArc(double x1, double y1, double x2, double y2, double phi,
                       int precision);
  GReturnValue FillTriangle(double x1, double y1, double x2, double y2,
                            double x3, double y3);
  GReturnValue FillRectangle(double x1, double y1, double x2, double y2);
  GReturnValue FillEllipseArc(double x1, double y1, double a, double b,
                              double phi1, double phi2, bool bFillTriangle);

  GReturnValue SetColor(unsigned char r, unsigned char g, unsigned char b);
  GReturnValue SetBackgroundColor(unsigned char r, unsigned char g,
                                  unsigned char b);
  GReturnValue SetThickness(double uThickness);
  GReturnValue PrintText(double x1, double y1, const std::string &sPosition,
                         const std::string &sText);
  GReturnValue PrintComment(const std::string &sText);
  GReturnValue Export(const std::string &sText, export_type format);
  GReturnValue SetFontSize(unsigned char size);

private:
  void PrintToOutput(const std::string &s);
  double LINE_WIDTH;
  std::ostream &OUTPUT;
  double m_dCurrentLineWidth;
  int m_iCurrentFontSize;
  std::string m_sCurrentColor;
  int m_nX; // width
  int m_nY; // height
};

#endif // !defined(SVGOUTPUT_H)
