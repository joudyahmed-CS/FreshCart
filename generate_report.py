"""Fill docs/TEAM.csv (English full names), then run this script.
Report generation only requires: python -m pip install reportlab
"""
from pathlib import Path
import csv
import reportlab
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.lib import colors
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.pagesizes import A4
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, Image, PageBreak
ROOT=Path(__file__).resolve().parents[1]
FONT_DIR=Path(reportlab.__file__).resolve().parent/'fonts'
pdfmetrics.registerFont(TTFont('FCRegular',str(FONT_DIR/'Vera.ttf')))
pdfmetrics.registerFont(TTFont('FCBold',str(FONT_DIR/'VeraBd.ttf')))
GREEN=colors.HexColor('#24745b'); INK=colors.HexColor('#1c302a'); MUTED=colors.HexColor('#66786e')
styles=getSampleStyleSheet()
for name,font,size,leading,color in [('TitleFC','FCBold',30,34,INK),('HeadingFC','FCBold',16,20,GREEN),('BodyFC','FCRegular',10,15,INK),('SmallFC','FCRegular',8.5,12,MUTED)]:
 styles.add(ParagraphStyle(name=name,fontName=font,fontSize=size,leading=leading,textColor=color,spaceAfter=8,spaceBefore=10 if name=='HeadingFC' else 0))
def p(text,style='BodyFC'): return Paragraph(text,styles[style])
def table(rows,widths):
 t=Table([[p(str(x),'SmallFC') for x in row] for row in rows],colWidths=widths,hAlign='LEFT')
 t.setStyle(TableStyle([('BACKGROUND',(0,0),(-1,0),colors.HexColor('#eaf3ed')),('VALIGN',(0,0),(-1,-1),'TOP'),('LINEBELOW',(0,0),(-1,-1),0.4,colors.HexColor('#e4e9e6')),('LEFTPADDING',(0,0),(-1,-1),10),('RIGHTPADDING',(0,0),(-1,-1),10),('TOPPADDING',(0,0),(-1,-1),8),('BOTTOMPADDING',(0,0),(-1,-1),7)]))
 return t
with (ROOT/'docs/TEAM.csv').open(newline='',encoding='utf-8-sig') as f:
 members=[r for r in csv.DictReader(f) if any((v or '').strip() for v in r.values())]
complete=3<=len(members)<=5 and all(all((v or '').strip() for v in m.values()) for m in members)
status='PROJECT REPORT' if complete else 'DRAFT REPORT - TEAM DETAILS REQUIRED'
path=ROOT/'docs/FreshCart_Project_Report.pdf'
doc=SimpleDocTemplate(str(path),pagesize=A4,rightMargin=42,leftMargin=42,topMargin=42,bottomMargin=40,title='FreshCart - Mini Supermarket Billing System',author='FreshCart project team')
story=[p('FRESHCART  /  COURSE PROJECT','SmallFC'),p('FreshCart','TitleFC'),p('Mini Supermarket Billing System','HeadingFC'),p(status,'SmallFC'),p('Faculty of Computers and Information Technology | Innovation University','SmallFC'),p('A C++17 supermarket application with a Qt desktop interface and a separate console entry point. It calculates item totals, applies member discounts, accepts cash, checks stock and generates receipts. Both applications share the same billing functions.'),p('Team members','HeadingFC')]
rows=[['Full name','Student ID','Actual contribution']]
rows += [[m['Full name'],m['Student ID'],m['Actual contribution']] for m in members] if members else [[f'Member {i} - complete','Complete','Complete'] for i in range(1,4)]
story += [table(rows,[170,95,246])]
if not complete: story += [p('Team size: 3-5 students. Fill TEAM.csv and regenerate this report before submission. Do not submit placeholder details.','SmallFC')]
story += [p('Behavior and course concepts','HeadingFC'),p('The console provides six numbered options, cin/cout interaction, a switch menu and a do-while loop. For loops process products and bills. Logical conditions validate input; a nested if selects the discount rate. Constants define discount thresholds. Arithmetic includes price x quantity, subtotal, percentage discount and cash change.'),p('Registered members receive 5% from EGP 500 and 10% from EGP 1,000. Non-members receive no discount. The higher rate replaces the lower rate. All money is stored in integer piastres. Stock is deducted only after checkout succeeds.'),p('Scope and verification','HeadingFC'),p('Sales and inventory changes last for the current session. Receipts can be exported as text files. Prices are illustrative. No database is used. The GUI is an enhancement; the console satisfies the mandatory application format.'),p('Compiled on Linux using GCC 14.2 and Qt 6.8.2. Core: 31 checks passed. GUI: three sales, filters, cancellation and insufficient payment passed. Console: three independent runs and EOF handling passed. Windows/Visual Studio testing must still be completed on the team computer.','SmallFC')]
runs=[
 ('01','Member checkout / 5% discount','Whole Milk x2 and Basmati Rice x6; registered member enabled.', [('Subtotal','EGP 654.00'),('Discount','EGP 32.70 (5%)'),('Amount due','EGP 621.30'),('Cash / change','EGP 700.00 / EGP 78.70')], '04_completed_receipt.png','PASS: EGP 600 was rejected as insufficient cash. Entering EGP 700 completed the sale, reduced stock and reset the cart.'),
 ('02','Standard checkout / exact cash','Fresh Bananas x2; registered member disabled.', [('Subtotal','EGP 76.00'),('Discount','EGP 0.00'),('Amount due','EGP 76.00'),('Cash / change','EGP 76.00 / EGP 0.00')], '05_standard_receipt.png','PASS: The second sale used exact cash and no discount. Both receipts are available. Cumulative net revenue is EGP 697.30.'),
 ('03','Gold checkout / 10% discount','Basmati Rice x11; registered member enabled.', [('Subtotal','EGP 1,045.00'),('Discount','EGP 104.50 (10%)'),('Amount due','EGP 940.50'),('Cash / change','EGP 940.50 / EGP 0.00')], '07_gold_receipt.png','PASS: The higher discount applied. The third sale completed with exact cash. Cumulative revenue is EGP 1,637.80 and 21 units were sold.')]
for num,title,inputs,results,image,conclusion in runs:
 story += [PageBreak(),p(f'TEST RUN {num}  /  ACTUAL APPLICATION CAPTURE','SmallFC'),p(title,'HeadingFC'),p(inputs),table([['Field','Observed result']]+results,[170,341]),Spacer(1,14),Image(str(ROOT/'docs/screenshots'/image),width=511,height=511*920/1440),Spacer(1,12),p(conclusion),p('The GUI test performs consecutive sales in one session. Full-resolution screenshots, reproducible tests and independent console output logs are included in the project package.','SmallFC')]
 if num=='03': story += [p('Submission completion','HeadingFC'),p('Add team details, demonstrate both applications in Visual Studio, upload the source to public GitHub and publish the LinkedIn post. Submit the actual GitHub and LinkedIn links. No public links have been created by this package.','SmallFC')]
def footer(canvas,doc):
 canvas.setStrokeColor(colors.HexColor('#dce5df')); canvas.line(42,32,A4[0]-42,32); canvas.setFont('FCRegular',8); canvas.setFillColor(MUTED); canvas.drawString(42,20,'FreshCart | C++ course project'); canvas.drawRightString(A4[0]-42,20,str(doc.page))
doc.build(story,onFirstPage=footer,onLaterPages=footer)
print(path)
