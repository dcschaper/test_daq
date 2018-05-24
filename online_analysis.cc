// g++ -o online_analysis online_analysis.cc `root-config --cflags --glibs`

#include "TGraph.h"
#include "TCanvas.h"

// int main(int argc, char** argv)

short buf[128][8];
TGraph gr[8];
TH2F empty("online","online analysis",128,0,128, 16384,0,65536);
TCanvas c("online");

void online_analysis(const char* fn)
{

  FILE* fp=fopen(fn,"r");
  empty.Draw();
  for (int i=0;i<8;++i) {
    for (int j=0;j<128;++j) {
      gr[i].SetPoint(j,j,5000*i);
    }
    gr[i].SetLineColor(i);
    gr[i].Draw("l");
  }
  while (fread(buf,2,1024,fp/*stdin*/)) {
    for (int i=0;i<8;++i) {
      for (int j=0;j<128;++j) {
        gr[i].SetPoint(j,j,buf[j][i]);
      }
    }
    c.Modified(); c.Update();
  }
}
