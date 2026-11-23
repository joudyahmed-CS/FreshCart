"""Optional report generation: install python-docx and Pillow, then run this file.
Reads docs/TEAM.csv and writes docs/FreshCart_Project_Report.docx.
Export that Word file to PDF using Word or LibreOffice after reviewing it.
Python is not required for either C++ application.
"""
from pathlib import Path
import csv
from docx import Document
from docx.shared import Inches,Pt,RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT,WD_CELL_VERTICAL_ALIGNMENT
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from PIL import Image

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'docs'
OUT.mkdir(exist_ok=True)
doc=Document();sec=doc.sections[0]
sec.page_width=Inches(8.5);sec.page_height=Inches(11)
sec.top_margin=Inches(.6);sec.bottom_margin=Inches(.6)
sec.left_margin=Inches(.65);sec.right_margin=Inches(.65)
sec.header_distance=Inches(.2);sec.footer_distance=Inches(.25)
for n in ['Normal','Title','Subtitle','Heading 1','Heading 2','Caption']:
 st=doc.styles[n];st.font.name='Arial';st.font.color.rgb=RGBColor(0,0,0)
 st._element.get_or_add_rPr().rFonts.set(qn('w:ascii'),'Arial')
 st._element.rPr.rFonts.set(qn('w:hAnsi'),'Arial')
 st.paragraph_format.space_after=Pt(6)
 st.paragraph_format.line_spacing=1.08
st=doc.styles['Normal'];st.font.size=Pt(11)
doc.styles['Title'].font.size=Pt(25);doc.styles['Title'].font.bold=True
doc.styles['Subtitle'].font.size=Pt(13)
for n,size in [('Heading 1',16),('Heading 2',12)]:
 st=doc.styles[n];st.font.size=Pt(size);st.font.bold=True
 st.paragraph_format.space_before=Pt(10);st.paragraph_format.keep_with_next=True
st=doc.styles['Caption'];st.font.size=Pt(9);st.font.italic=False;st.font.bold=False
st.paragraph_format.space_before=Pt(3);st.paragraph_format.space_after=Pt(7)
# Plain page number provides navigation without a decorative footer rule.
p=sec.footer.paragraphs[0];p.alignment=WD_ALIGN_PARAGRAPH.RIGHT
r=p.add_run();r.font.name='Arial';r.font.size=Pt(9)
f=OxmlElement('w:fldSimple');f.set(qn('w:instr'),'PAGE');r._r.addnext(f)

def para(text,style=None):
 p=doc.add_paragraph(text,style);return p

def heading(text):return para(text,'Heading 1')
def table(headers,rows,widths):
 t=doc.add_table(rows=1,cols=len(headers));t.alignment=WD_TABLE_ALIGNMENT.CENTER;t.autofit=False
 for c,w in zip(t.columns,widths):c.width=Inches(w)
 def fill(row,values,ishead=False):
  for i,(cell,text) in enumerate(zip(row.cells,values)):
   cell.width=Inches(widths[i]);cell.vertical_alignment=WD_CELL_VERTICAL_ALIGNMENT.CENTER
   p=cell.paragraphs[0];p.paragraph_format.space_before=Pt(0);p.paragraph_format.space_after=Pt(0)
   p.paragraph_format.line_spacing=1.03
   if i and len(str(text))<23:p.alignment=WD_ALIGN_PARAGRAPH.CENTER
   r=p.add_run(str(text));r.font.size=Pt(10);r.font.name='Arial';r.font.bold=ishead
   props=cell._tc.get_or_add_tcPr()
   border=OxmlElement('w:tcBorders')
   for side in ['top','left','bottom','right']:
    edge=OxmlElement('w:'+side);edge.set(qn('w:val'),'single');edge.set(qn('w:sz'),'4');edge.set(qn('w:color'),'D9D9D9');border.append(edge)
   props.append(border)
   margins=OxmlElement('w:tcMar')
   for side,value in [('top','95'),('bottom','95'),('left','110'),('right','110')]:
    e=OxmlElement('w:'+side);e.set(qn('w:w'),value);e.set(qn('w:type'),'dxa');margins.append(e)
   props.append(margins)
   if ishead:
    shade=OxmlElement('w:shd');shade.set(qn('w:fill'),'E8F0F5');props.append(shade)
  # Keep each record intact while allowing a whole table to split if required.
  trPr=row._tr.get_or_add_trPr();cant=OxmlElement('w:cantSplit');trPr.append(cant)
 fill(t.rows[0],headers,True)
 repeat=OxmlElement('w:tblHeader');t.rows[0]._tr.get_or_add_trPr().append(repeat)
 for vals in rows:fill(t.add_row(),vals)
 para('').paragraph_format.space_after=Pt(1)
 return t

def picture(path,width=6.8,crop=None):
 p=doc.add_paragraph();p.alignment=WD_ALIGN_PARAGRAPH.CENTER
 p.paragraph_format.space_after=Pt(0);p.paragraph_format.space_before=Pt(3)
 shape=p.add_run().add_picture(str(path),width=Inches(width))
 if crop:
  # Crop is a document layout instruction; the original screenshot bytes remain unchanged.
  l,t,r,b=crop
  fill=shape._inline.graphic.graphicData.pic.blipFill
  src=OxmlElement('a:srcRect')
  for k,v in zip(['l','t','r','b'],crop):src.set(k,str(v))
  fill.insert(1,src)
  iw,ih=Image.open(path).size
  shape.height=int(Inches(width)*(ih*(100000-t-b))/(iw*(100000-l-r)))
  shape._inline.docPr.set('descr','FreshCart desktop application running on the team Windows computer')
 else:shape._inline.docPr.set('descr',path.stem.replace('_',' '))
 return p

para('FreshCart Project Report','Title')
para('Mini Supermarket Billing System','Subtitle')
para('Innovation University  |  Faculty of Computers and Information Technology')
para('24 November 2026')
para('FreshCart is a C++17 application for preparing supermarket bills. It calculates item totals and member discounts, checks stock, accepts cash and produces receipts. A menu driven console version and a Qt desktop version use the same billing functions.')
heading('Team members')
with (ROOT/'docs/TEAM.csv').open(newline='',encoding='utf-8-sig') as team_file:
 team_rows=[[r['Full name'],r['Student ID']] for r in csv.DictReader(team_file)]
table(['Full name','Student ID'],team_rows,[5.2,2.0])
heading('Desktop application')
picture(ROOT/'docs/screenshots/08_windows_application.png',6.45,crop=(417,0,5833,4630))
para('Figure 1  FreshCart launched on the team Windows computer after a successful MSVC build','Caption')
para('The Windows build used Visual Studio Community 2026 C++ tools and Qt 6.12.1. The final build in VS Code completed with exit code 0, and the Qt runtime files were deployed before launch.')
para('Scope  Prices are sample values in EGP. Sales and stock changes remain in memory for the current session. Receipts can be exported as text files before closing the application.')

doc.add_page_break();heading('Programming concepts and implementation')
para('The console is the course entry point. Its six options are Show products, Set cart quantity, View cart and member status, Checkout, Sales summary and Exit. The desktop interface adds buttons and receipt previews while using the same calculations.')
rows=[
 ('Menu and interaction','Welcome message and six numbered options using cin and cout','console.cpp'),
 ('Variables and constants','Integer quantities, bool membership, strings and constexpr discount limits','store.h'),
 ('Arithmetic','Price times quantity, accumulated subtotal, percentage discount and cash change','store.cpp'),
 ('Decisions and logical operators','Membership and minimum subtotal combined with &&; range checks use ||','store.cpp'),
 ('Nested if','After eligibility is checked, choose the 5% or 10% member rate','store.cpp'),
 ('Switch statement','case, break and default process the main menu','console.cpp'),
 ('Repeated operations','do while repeats the menu; for loops process products and update stock','console.cpp and store.cpp'),
 ('Input validation','Reject invalid menu text, negative or excessive quantities and insufficient cash; retry input','console.cpp and store.cpp'),
 ('Readable results','Subtotal, discount, total, payment, change and eligibility status appear in the receipt','Both interfaces'),
]
table(['Concept','Implementation','Location'],rows,[1.5,4.15,1.55])
heading('Discount rules')
para('Registered members receive 5% from EGP 500 up to below EGP 1,000, and 10% from EGP 1,000. Other orders receive no discount. The 10% rate replaces the 5% rate. There are no added taxes or fees.')
heading('Correctness and testing')
para('Money is stored as integer piastres. Discounts round to the nearest piastre using half up rounding. Checkout validates every quantity and the cash amount before changing stock or recording a sale. The cart resets only after a successful payment.')
para('The original automated core suite passed 31 checks, including discount boundaries, rounding, empty carts, invalid quantities, failed payment and stock updates. Three console process runs and the automated Qt workflow also passed. Windows compilation and GUI launch were subsequently verified on the team computer.')
para('The three transaction screenshots on the following pages are actual captures from the original automated Qt runs on Linux. Windows launch evidence is shown in Figure 1. Full resolution images and console output logs accompany the source package.','Caption')

runs=[
 ('Test run 1 Member discount','Add Whole Milk twice and Basmati Rice six times, enable Registered member and open the payment dialog.',
  [('Products','Whole Milk x2 and Basmati Rice x6'),('Subtotal','EGP 654.00'),('Discount','EGP 32.70 at 5%'),('Total due','EGP 621.30'),('Cash received','EGP 700.00'),('Change','EGP 78.70')],
  '04_completed_receipt.png','An initial cash amount of EGP 600 was rejected. After entering EGP 700, the sale completed, stock was reduced and the cart reset. The receipt records the correct total and change.'),
 ('Test run 2 Standard pricing','Return to Point of sale, add Fresh Bananas twice and leave Registered member disabled. Pay the exact total.',
  [('Products','Fresh Bananas x2'),('Subtotal','EGP 76.00'),('Discount','EGP 0.00'),('Total due','EGP 76.00'),('Cash received','EGP 76.00'),('Change','EGP 0.00')],
  '05_standard_receipt.png','The second transaction completed without a discount. The receipt shows zero change for exact cash. Both receipts remain selectable, and cumulative net revenue is EGP 697.30.'),
 ('Test run 3 Gold discount','Return to Point of sale, add Basmati Rice eleven times and enable Registered member. Confirm payment using the exact total.',
  [('Products','Basmati Rice x11'),('Subtotal','EGP 1045.00'),('Discount','EGP 104.50 at 10%'),('Total due','EGP 940.50'),('Cash received','EGP 940.50'),('Change','EGP 0.00')],
  '07_gold_receipt.png','The 10% rate was applied because the member subtotal exceeded EGP 1,000. The sale completed with zero change. Across all three sales, net revenue is EGP 1637.80 and 21 items were sold.')]
for i,(title,instructions,results,image,result) in enumerate(runs,2):
 doc.add_page_break();heading(title);para(instructions)
 table(['Field','Observed result'],results,[2.1,5.1])
 picture(ROOT/'docs/screenshots'/image,6.95)
 para(f'Figure {i}  Completed receipt from an actual Qt application test run','Caption')
 heading('Result')
 para('Passed. '+result)
 if i==4:
  para('The separate console validation for the gold case used EGP 1000.00 cash and correctly returned EGP 59.50. Its log is included in docs/test-runs/03_gold_validation.txt.','Caption')

# Remove accidental paragraph borders and keep black headings stable across renderers.
for p in doc.paragraphs:
 pp=p._p.pPr
 if pp is not None:
  for border in list(pp.findall(qn('w:pBdr'))):pp.remove(border)
for style in doc.styles:
 pp=style._element.find(qn('w:pPr'))
 if pp is not None:
  for border in list(pp.findall(qn('w:pBdr'))):pp.remove(border)
doc.styles['Subtitle'].font.italic=False
doc.styles['Title'].font.underline=False
doc.core_properties.title='FreshCart Project Report'
doc.core_properties.subject='Mini Supermarket Billing System'
doc.core_properties.author='Joudy Ahmed Sayed and Amira Tamer Mandour'
doc.core_properties.keywords='C++ Qt console supermarket course project'
path=OUT/'FreshCart_Project_Report.docx';doc.save(path)
shutil=None
print(path)
