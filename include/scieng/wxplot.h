#ifndef SCIWXPLOT_H
#define SCIWXPLOT_H
#include"plot/plot.h"
#include"wxrenderer.h"

namespace sci
{
	namespace plot
	{
		class wxPlotCanvasPanel : public ::sci::graphics::wxGraphicsPanel
		{
		public:
			wxPlotCanvasPanel(wxWindow* parent, int id = wxID_ANY)
				: ::sci::graphics::wxGraphicsPanel(parent, id)
			{
			}
			PlotCanvas* getCanvas()
			{
				return &m_plotCanvas;
			}
		private:
			virtual void OnPaint(wxPaintEvent& event) override
			{
				wxPaintDC dc(this);
				::sci::graphics::wxDcRenderer renderer(&dc, GetClientSize(), perInch(FromDIP(96)));
				m_plotCanvas.render(renderer, perInch(FromDIP(96)));
			}
			PlotCanvas m_plotCanvas;
		};




		inline bool wxWriteToVectorFile(sci::plot::PlotCanvas& canvas, sci::string filename, millimetre width, millimetre height)
		{
			//get the extension
			wxFileName fullfile = wxString(sci::toNativeUnicode(filename));
			wxString extension = fullfile.GetExt().Lower();

			bool result = true;


			if (extension == "ps")
			{
				//here we redraw the plot like OnPaint but using a postscript DC.
				wxPrintData setupdata;
				setupdata.SetColour(true);
				setupdata.SetFilename(sci::toNativeUnicode(filename));
				//setupdata.SetPaperId(wxPAPER_A4);
				setupdata.SetPaperId(wxPAPER_NONE);

				//ps uses the default unit of pt(1/72 inch)
				perInch scale(72);
				int widthDevice = (width * scale).value<unitless>();
				int heightDevice = (height * scale).value<unitless>();
				setupdata.SetPaperSize(wxSize(widthDevice, heightDevice));
				setupdata.SetPrintMode(wxPRINT_MODE_FILE);
				//setupdata.SetQuality(wxPRINT_QUALITY_HIGH); //doesn't seem to do anything
				wxPostScriptDC psdc(setupdata);
				result = psdc.StartDoc(sci::toNativeUnicode(sU("Writing ") + filename));
				if (result == false)
					return result;
				sci::graphics::wxDcRenderer renderer(&psdc, wxSize(widthDevice, heightDevice), scale);
				canvas.render(renderer, scale);
				psdc.EndDoc();
			}
#ifdef _WIN32
			else if (extension == "emf")
			{

				//emf uses the default unit of twip(1/1440 inch, 1/20 pt)
				perInch scale(1440);
				int widthDevice = (width * scale).value<unitless>();
				int heightDevice = (height * scale).value<unitless>();
				wxMetafileDC metadc(sci::toNativeUnicode(filename), widthDevice, heightDevice);
				sci::graphics::wxDcRenderer renderer(&metadc, wxSize(widthDevice, heightDevice), scale);
				canvas.render(renderer, scale);;//0 gives vector output
				//close the file - note this gives us a copy of the file in memory which we must delete
				wxMetafile* metafile = metadc.Close();
				result = metafile != NULL;
				delete metafile;
				//we must call this function to make the file importable into other software
				int minx = metadc.MinX();
				int maxx = metadc.MaxX();
				int miny = metadc.MinY();
				int maxy = metadc.MaxY();
				//wxMakeMetaFilePlaceable(minx,miny,maxx,maxy);
			}
#endif
			else //svg is the default
			{
				//we can set the units for a wxSVGFileDC, which is nice. It needs to be in
				//dpi, so I'm going to use twips, for the units, like eps (1/1440 inch, 1/20 pt)
				perInch scale(1440);
				int widthDevice = (width * scale).value<unitless>();
				int heightDevice = (height * scale).value<unitless>();
				wxSVGFileDC dc(sci::toNativeUnicode(filename), widthDevice, heightDevice, scale.value<perInch>());
				sci::graphics::wxDcRenderer renderer(&dc, wxSize(widthDevice, heightDevice), scale);
				canvas.render(renderer, scale);
				result = true;
			}

			return result;
		}

		inline bool wxWriteToRasterFile(sci::plot::PlotCanvas& canvas, sci::string filename, int width, int height, perInch resolution)
		{
			//get the extension
			wxFileName fullfile = wxString(sci::toNativeUnicode(filename));
			wxString extension = fullfile.GetExt().Lower();

			bool result = true;

			//load the image handlers
			wxInitAllImageHandlers();
			//create a wxBitmapType to define the type saved as
			//use PNG as default
			wxBitmapType type = wxBITMAP_TYPE_PNG;
			if (extension == wxT("jpg") || extension == wxT("jpeg"))
				type = wxBITMAP_TYPE_JPEG;
			else if (extension == wxT("bmp"))
				type = wxBITMAP_TYPE_BMP;
			else if (extension == wxT("tif") || extension == wxT("tiff"))
				type = wxBITMAP_TYPE_TIF;
			else if (extension == wxT("pcx"))
				type = wxBITMAP_TYPE_PCX;
			else if (extension == wxT("xpm"))
				type = wxBITMAP_TYPE_XPM;
			else if (extension == wxT("xbm"))
				type = wxBITMAP_TYPE_XBM;
			else if (extension == wxT("pnm"))
				type = wxBITMAP_TYPE_PNM;


			//create a wxMemoryDC which will be linked to the bitmap. We'll use this to draw to the bitmap
			//except if using AGG then we create an image instead which we'll need to copy to the bitmap
			wxMemoryDC memdc;
			wxBitmap bitmap(width, height, 32);
			if (!bitmap.IsOk())
				return false;
			memdc.SelectObject(bitmap);
			//fill the bitmap with white giving a white background for plplot
			//or to show blank if there are no plots
			memdc.FloodFill(0, 0, *wxWHITE, wxFLOOD_BORDER);

			sci::graphics::wxDcRenderer renderer(&memdc, wxSize(width, height), resolution);
			canvas.render(renderer, resolution);


			//reselect null bitmap for the memdc
			memdc.SelectObject(wxNullBitmap);

			//write the bitmap to file
			result = bitmap.SaveFile(sci::toNativeUnicode(filename), type);


			return result;
		}
	}
}

#endif