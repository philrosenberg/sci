#ifndef SCIWXRENDERER_H
#define SCIWXRENDERER_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define SVECTOR_MUST_RESET_CRT_SECURE_NO_WARNINGS
#endif

#include<vector>
#pragma warning(push, 0)
#include<wx/wx.h>
#include<wx/colour.h>
#include<wx/scrolwin.h>
#include<limits>
#include<wx/print.h>
#include<wx/printdlg.h>
#include<wx/filename.h>
#include<wx/dcps.h>
#include <wx/dcsvg.h>
#include<wx/metafile.h>
#include<wx/gdicmn.h>
#include<wx/dcgraph.h>
#pragma warning(pop)
#include"graphics.h"
#include"string.h"
#include"stringfuncs.h"
#include"Units.h"
#include<utility>
#include<array>
#include<map>
#include<optional>
#include<span>

namespace sci
{
	namespace graphics
	{
		class wxDcRenderer : public Renderer
		{
		public:
			wxDcRenderer(wxDC* dc, wxSize size, perMillimetre scale, sci::string escapeSequence = sU("#"))
				:m_dc(dc), m_height(size.GetHeight()), m_width(size.GetWidth()), m_scale(scale), m_fontSize(dc->GetFont().IsOk() ? dc->GetFont().GetPointSize() : 0.0)
			{
				pushState();
			}
			~wxDcRenderer()
			{
				popState();
			}
			wxDC* getDc()
			{
				return m_dc;
			}

			const wxDC* getDc() const
			{
				return m_dc;
			}

			virtual void pushState()
			{
				m_stateStack.push_back({ m_dc->GetPen(),
					m_penDashes,
					m_dc->GetBrush(),
					m_dc->GetTextForeground(),
					m_dc->GetTextBackground(),
					m_dc->GetFont(),
					m_fontSize});
			}

			virtual void popState()
			{
				try
				{
					wxPen pen = m_stateStack.back().m_pen;
					m_penDashes = m_stateStack.back().m_penDashes;
					m_dc->SetPen(pen);
					m_dc->SetBrush(m_stateStack.back().m_brush);
					m_dc->SetTextForeground(m_stateStack.back().m_textForeground);
					m_dc->SetTextBackground(m_stateStack.back().m_textBackground);
					m_dc->SetFont(m_stateStack.back().m_font);
					m_fontSize = m_stateStack.back().m_fontSize;
				}
				catch (...)
				{
					m_stateStack.pop_back();
				}
				m_stateStack.pop_back();
			}

			virtual void applyClippingRegion(Point point, Distance distance) override
			{
				wxRegion region(getWxPoint(point), getWxPoint(point, distance));
				m_clippingRegions.push_back(region);
				m_dc->SetClippingRegion(region);
			}
			virtual void setBrush(const HlsColour& colour) override
			{
				setBrush(colour.convertToRgb());
			}
			virtual void setBrush(const RgbColour& colour) override
			{
				m_dc->SetBrush(wxBrush(getWxColour(colour)));
			}
			virtual void setPen(const HlsColour& colour, const Length &thickness, const std::vector<Length>& dashes = std::vector<Length>(0)) override
			{
				setPen(colour.convertToRgb(), thickness, dashes);
			}
			virtual void setPen(const RgbColour& colour, const Length &thickness, const std::vector<Length>& dashes = std::vector<Length>(0)) override
			{
				m_dc->SetPen(wxNullPen); //this deselects the prvious pen, so if we've used
				//dashes, the dash array can now be invalidated.

				//If thickness is 0, turn off the pen
				if (thickness.getLength(m_width, m_height, m_scale) == 0.0)
				{
					m_dc->SetPen(*wxTRANSPARENT_PEN);
					return;
				}

				wxPen pen(getWxColour(colour), thickness.getLength(m_width, m_height, m_scale), dashes.size()==0 ? wxPENSTYLE_SOLID : wxPENSTYLE_USER_DASH);
				if (dashes.size() > 0)
				{
					wxDash* dashesCopy = new wxDash[dashes.size()];
					//m_penDashes.resize(0);
					//m_penDashes.reserve(dashes.size());
					for (size_t i=0; i<dashes.size(); ++i)
						dashesCopy[i] = wxDash(std::max(1.0, dashes[i].getLength(m_width, m_height, m_scale) / thickness.getLength(m_width, m_height, m_scale)));
						//dashesCopy[i] = dashes[i].getLength(m_width, m_height, m_scale);
					pen.SetDashes(dashes.size(), dashesCopy);
				}

				m_dc->SetPen(pen);
			}
			virtual void setFont(sci::string facename, Length size, RgbColour colour, Renderer::FontFamily backupFamily = Renderer::FontFamily::defaultFont, bool bold = false, bool italic = false, bool underline = false, RgbColour backgroundColour = RgbColour(0,0,0,1)) override
			{
				setFont(std::vector<sci::string>{ facename }, size, colour, backupFamily, bold, italic, underline, backgroundColour);
			}
			virtual void setFont(std::vector<sci::string> facenames, Length size, RgbColour colour, Renderer::FontFamily backupFamily = Renderer::FontFamily::defaultFont, bool bold = false, bool italic = false, bool underline = false, RgbColour backgroundColour = RgbColour(0, 0, 0, 1)) override
			{
				pushState();
				wxFont font;
				for (auto& facename : facenames)
				{
					font = wxFont(); //clear the font - not sure if this is needed
					font.SetFaceName(toWxString(facename));
					if (font.IsOk())
						break;
				}
				if (!font.IsOk() || facenames.size() == 0)
				{
					font = wxFont(); //clear the font - not sure if this is needed
					font.SetFamily(getWxFontFamily(backupFamily));
				}
				m_fontSize = size.getLength(unitless(m_width) / m_scale, unitless(m_height) / m_scale);
				font.SetFractionalPointSize(m_fontSize.value<textPoint>());
				font.SetWeight(bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
				font.SetStyle(italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
				font.SetUnderlined(underline);
				m_dc->SetTextForeground(getWxColour(colour));
				m_dc->SetTextBackground(getWxColour(backgroundColour));
				m_dc->SetFont(font);
			}

			virtual void line(const Point& p1, const Point& p2) override
			{
				m_dc->DrawLine(getWxPoint(p1), getWxPoint(p2));
			}

			virtual void line(const Point& point, const Distance& distance) override
			{
				m_dc->DrawLine(getWxPoint(point), getWxPoint(point, distance));
			}

			virtual TextMetric text(const sci::string& str, const Point& position, unitless horizontalAlignment, unitless verticalAlignment) override
			{
				wxPoint wxPosition = getWxPoint(position);
				wxString wxStr = toWxString(str);
				wxCoord width, ascentPlusDescent, descent, leading;
				m_dc->GetTextExtent(wxStr, &width, &ascentPlusDescent, &descent, &leading);
				wxCoord ascent = ascentPlusDescent - descent;
				//these values are based on some tests on windows. I don't really know how good they are
				//leading = descent * 0.32 + 1;
				//descent = descent * 1.61 + 1;
				wxPosition.x -= int(width * horizontalAlignment.value<unitless>());
				wxPosition.y -= int(ascent * verticalAlignment.value<unitless>());
				m_dc->DrawText(wxStr, wxPosition);
				return TextMetric(millimetre(unitless(width) / m_scale),
					millimetre(unitless(ascentPlusDescent - descent) / m_scale),
					millimetre(unitless(descent) / m_scale),
					millimetre(unitless(ascentPlusDescent - descent) / m_scale),
					millimetre(unitless(descent) / m_scale));
			}

			virtual TextMetric rotatedText(const sci::string& str, const Point& position, unitless horizontalAlignment, unitless verticalAlignment, degree rotation) override
			{
				wxPoint wxPosition = getWxPoint(position);
				wxString wxStr = toWxString(str);
				//wxSize extent = m_dc->GetTextExtent(wxStr);
				wxCoord width, ascentPlusDescent, descent, leading;
				m_dc->GetTextExtent(wxStr, &width, &ascentPlusDescent, &descent, &leading);
				wxCoord ascent = ascentPlusDescent - descent;

				wxPosition.x -= int((width * horizontalAlignment.value<unitless>()) * sci::cos(rotation).value<unitless>() + (ascent * verticalAlignment.value<unitless>()) * sci::sin(rotation).value<unitless>());
				wxPosition.y -= -int((width * horizontalAlignment.value<unitless>()) * sci::sin(rotation).value<unitless>() + (ascent * verticalAlignment.value<unitless>()) * sci::cos(rotation).value<unitless>());

				m_dc->DrawRotatedText(wxStr, wxPosition, rotation.value<degree>());
				return TextMetric(millimetre(unitless(width) / m_scale), millimetre(unitless(ascentPlusDescent - descent) / m_scale), millimetre(unitless(descent) / m_scale));
			}
			virtual void elipse(const Point& position, const Distance& radius, unitless xAlignemnt = unitless(0.5), unitless yAlignment = unitless(0.5)) override
			{
				Distance diameter = radius * unitless(2.0);
				wxPoint wxPosition = getWxPoint(position - diameter * std::array<unitless, 2>{xAlignemnt, yAlignment});
				wxSize size = getWxSize(diameter);
				m_dc->DrawEllipse(wxPosition, size);
			}
			virtual void rectangle(const Point& position, const Distance& size, unitless xAlignemnt = unitless(0.0), unitless yAlignment = unitless(0.0)) override
			{
				wxPoint wxPosition = getWxPoint(position - size * std::array<unitless,2>{xAlignemnt, yAlignment});
				wxSize wxsize = getWxSize(size);
				m_dc->DrawRectangle(wxPosition, wxsize);
			}
			virtual void rectangle(const Point& corner1, const Point& corner2) override
			{
				//override this for consistency with polygon and integrer resolution
				wxPoint wxCorner1 = getWxPoint(corner1);
				wxPoint wxCorner2 = getWxPoint(corner2);
				//this offsetting is based on the plot grid test and makes the rectangles
				//the same as a polygon with the corners the same as the rectangls, it may
				//need some further checking as I'm not sure why it does what it does.
				wxCorner1.y += 1;
				wxCorner2.y += 1;
				wxSize wxsize(wxCorner2.x- wxCorner1.x, wxCorner2.y - wxCorner1.y);
				wxsize.x += wxsize.x < 0 ? -1 : 1;
				wxsize.y += wxsize.y < 0 ? -2 : 2;
				m_dc->DrawRectangle(wxCorner1, wxsize);
			}
			void scaleFontSize(unitless scale) override
			{
				m_fontSize *= scale;
				auto font = m_dc->GetFont();
				font.SetFractionalPointSize(m_fontSize.value<textPoint>());
				m_dc->SetFont(font);
			}
			virtual void polyLine(const std::vector<Point>& points)
			{
				if (points.size() == 0)
					return;
				std::vector<wxPoint> wxPoints;
				wxPoints.reserve(points.size());
				for (auto &p : points)
					wxPoints.push_back(getWxPoint(p));
				m_dc->DrawLines(points.size(), &wxPoints[0]);
			}

			virtual void polygon(const std::vector<Point>& points)
			{
				if (points.size() == 0)
					return;
				std::vector<wxPoint> wxPoints;
				wxPoints.reserve(points.size());
				for (auto& p : points)
					wxPoints.push_back(getWxPoint(p));
				m_dc->DrawPolygon(points.size(), &wxPoints[0]);
			}

			virtual degree getAngle(const Distance& distance) const
			{
				double x = distance.getX(m_width, m_height, m_scale);
				double y = distance.getY(m_width, m_height, m_scale);
				return sci::atan2(unitless(-y), unitless(x));
			}
		private:
			wxString toWxString(const sci::string &str)
			{
				//wxWidgets matches it's unicode version to the native std::wstring encoding
				//so this should be the easiest format to pass in
#ifdef _WIN32
				return wxString(sci::toNativeUnicode(str));
#else
				return wxString((wchar_t*)sci::toUtf32(str).c_str());
#endif
			}
			virtual void applyClippingRegion(Point corner1, Point corner2) override
			{
				wxRegion region(getWxPoint(corner1), getWxPoint(corner2));
				m_clippingRegions.push_back(region);
				m_dc->SetClippingRegion(region);
			}

			virtual void applyClippingRegion(std::span<const Point> points) override
			{
				std::vector<wxPoint> wxPoints(points.size());
				for (size_t i = 0; i < wxPoints.size(); ++i)
					wxPoints[i] = getWxPoint(points[i]);
				wxRegion region(wxPoints.size(), &wxPoints[0]);
				m_clippingRegions.push_back(region);
				m_dc->SetClippingRegion(region);
			}

			virtual void popClippingRegion() override
			{
				m_dc->DestroyClippingRegion();
				if (m_clippingRegions.size() == 0)
					return;
				m_clippingRegions.resize(m_clippingRegions.size() - 1);
				for (auto& r : m_clippingRegions)
					m_dc->SetClippingRegion(r);
			}

			struct State
			{
				wxPen m_pen;
				std::vector<wxDash> m_penDashes;
				wxBrush m_brush;
				wxColour m_textForeground;
				wxColour m_textBackground;
				wxFont m_font;
				textPoint m_fontSize; //stored here in double form as wxFont stores it as an int
			};
			wxPoint getWxPoint(const Point& point)
			{
				return wxPoint(int(point.getX(m_width, m_height, m_scale)), int(point.getY(m_width, m_height, m_scale)));
			}
			wxPoint getWxPoint(const Point& point, const Distance &distance)
			{
				double xStart = point.getX(m_width, m_height, m_scale);
				double yStart = point.getY(m_width, m_height, m_scale);
		
				return wxPoint(int(xStart + distance.getX(m_width, m_height, m_scale)),
					int(yStart + distance.getY(m_width, m_height, m_scale)));
			}
			wxSize getWxSize(const Distance& distance)
			{
				return wxSize(distance.getX(m_width, m_height, m_scale), distance.getY(m_width, m_height, m_scale));
			}
			wxColour getWxColour(const RgbColour& colour)
			{
				return wxColour(colour.r() * 255.99999, colour.g() * 255.99999, colour.b() * 255.99999, colour.a() * 255.99999);
			}
			wxFontFamily getWxFontFamily(Renderer::FontFamily family)
			{
				if (Renderer::FontFamily::sansSerif == family)
					return wxFONTFAMILY_SWISS;
				if (Renderer::FontFamily::serif == family)
					return wxFONTFAMILY_ROMAN;
				if (Renderer::FontFamily::cursive == family)
					return wxFONTFAMILY_SCRIPT;
				if (Renderer::FontFamily::mono == family)
					return wxFONTFAMILY_TELETYPE;
				if (Renderer::FontFamily::fantasy == family)
					return wxFONTFAMILY_DECORATIVE;
				return wxFONTFAMILY_DEFAULT;
			}

			virtual TextMetric getUnformattedTextExtent(const sci::string& str) override
			{
				wxCoord width, ascentPlusDescent, descent, leading;
				wxString wxStr = str.length() > 0 ? toWxString(str) : wxString("M"); // ensure the string isn't empty so it gets a non-zero height
				m_dc->GetTextExtent(wxStr, &width, &ascentPlusDescent, &descent, &leading);
				wxCoord ascent = ascentPlusDescent - descent;
				if (str.length() == 0)
					width = 0;//if the length was zero set the width to zero rather than the width of the dummy string from above
				return TextMetric(millimetre(unitless(width) / m_scale),
					millimetre(unitless(ascentPlusDescent - descent) / m_scale),
					millimetre(unitless(descent) / m_scale),
					millimetre(unitless(ascentPlusDescent - descent) / m_scale),
					millimetre(unitless(descent) / m_scale));
			}

			void unformattedRotatedText(const wxString& str, const std::array<double, 2> &position, degree rotation)
			{
				//wxSize extent = m_dc->GetTextExtent(wxStr);
				/*wxCoord width, height, descent, leading;
				unformattedTextExtent(str, width, height, descent, leading);

				position.x -= (leading + width * horizontalAlignment.value<unitless>()) * sci::cos(rotation).value<unitless>() + (descent + height * verticalAlignment.value<unitless>()) * sci::sin(rotation).value<unitless>();
				position.y -= -(leading + width * horizontalAlignment.value<unitless>()) * sci::sin(rotation).value<unitless>() + (descent + height * verticalAlignment.value<unitless>()) * sci::cos(rotation).value<unitless>();

				m_dc->DrawRotatedText(wxStr, wxPosition, rotation.value<degree>());
				return Distance(millimetre(unitless(x) / m_scale), millimetre(unitless(y) / m_scale));*/
			}

			millimetre getFontSize() const override
			{
				return textPoint(m_fontSize);
			}

			wxDC* m_dc;
			double m_height;
			double m_width;
			perMillimetre m_scale;
			std::vector<wxDash> m_penDashes;
			std::vector<State> m_stateStack;
			textPoint m_fontSize; //stored here in double form as wxFont stores it as an int
			std::vector<wxRegion> m_clippingRegions;
		};

		
		class wxGraphicsPanel : public wxPanel
		{
		public:
			wxGraphicsPanel(wxWindow* parent, int id = wxID_ANY)
				:wxPanel(parent, id)
			{
				Connect(wxEVT_PAINT, wxPaintEventHandler(wxGraphicsPanel::OnPaint));
				Connect(wxEVT_SIZE, wxSizeEventHandler(wxGraphicsPanel::OnResize));
				Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(wxGraphicsPanel::OnErase));
			}
		private:
			virtual void OnPaint(wxPaintEvent& event)
			{
				//override this function, but this is the kind of thing to do
				wxPaintDC dc(this);
				wxDcRenderer renderer(&dc, GetClientSize(), perInch(FromDIP(96)));
			}
			virtual void OnErase(wxEraseEvent& event)
			{
				//deliberately do nothing
			}
			void OnResize(wxSizeEvent& event)
			{
				//refresh the whole panel so it is all redrawn, not just the invalidated area
				Refresh();
			}
			//DECLARE_EVENT_TABLE();
		};

		template<class PANEL = wxGraphicsPanel>
		class wxGraphicsFrame : public wxFrame
		{
		public:
			wxGraphicsFrame(wxFrame* parent)
				: wxFrame(parent, wxID_ANY, "Graphics frame", wxDefaultPosition, FromDIP(wxSize(800, 600)))
			{
				m_panel = new PANEL(this);
			}
			~wxGraphicsFrame()
			{
			}
			PANEL* getPanel()
			{
				return m_panel;
			}
		private:
			PANEL* m_panel;

		};
	}
}

#endif