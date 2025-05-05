//*****************************
//Get Json Data
function GetJData(uri,onSuccess){
 let xhr=new XMLHttpRequest();
 xhr.onload=function(){
  if(xhr.status===200){if(onSuccess)onSuccess(JSON.parse(xhr.responseText));}
    else{console.error(xhr);}};
 xhr.open("GET",uri);
 xhr.send();}
//PostRequest PostJRequest
function PostRequest(func,uri,data="{}"){
 console.log( "Post:", data );
 let xhr=new XMLHttpRequest();
 xhr.onload=function(){
  if(xhr.status===200)SetStatus(func,parseInt(xhr.responseText));
   else{console.error(xhr);}};
 xhr.open("POST",uri);
 xhr.setRequestHeader('Content-Type','application/json; charset=UTF-8');
 xhr.send(data);}
function PostJRequest(func,uri,jdata){
 PostRequest(func,uri,JSON.stringify(jdata));}
//Set Document-Title
function ChangeTitle(newTitle){
 document.title = newTitle;}
//*****************************
//change value of Cb
function ChChg(ele){ele.value=ele.checked?1:0;}
//Make DOM Elements (with Value)
function MkCb(val){return `<input type="checkbox" value=${val} ${val?"checked ":""}onclick="ChChg(this)">`;}
function MkSel(val){let r="";for(let i=1;i<=9;i++)r+=`<option${(val==i)?" selected":""}>${i}</option>`;return `<select>${r}</select>`;}	
function MkTm(val){return `<input type="time" value="${val}">`;}
//Make DOM Elements
function MkTxt(){return `<input type=text>`;}
function MkPw(){return `<input type=password>`;}
function MkNr(min_,max_,val){return `<input type=number${min_==undefined?"":` min=${min_}`}${max_==undefined?"":` max=${max_}`}${val==undefined?"":` value=${val}`}>`;}
function MkPin(){return MkNr(0,15);}
function MkPercent(){return MkNr(0,100);}
function MkUint(){return MkNr(0,10000);}
function MkMode(){
 const nr2Name=["Station","Accesspoint","Station + Accesspoint"];
 let r="";for(let i=1;i<=3;i++)r+=`<option value=${i}>${nr2Name[i-1]}</option>`;
 return `<select>${r}</select>`;}	
function MkCell(inner,extra=""){return `<td>${inner}${extra!=""?" ":""}${extra}</td>`;}
function MkRow(inner,id){return `<tr${id==undefined?"":" id=${id}"}>${inner}</tr>`;}
//Get Value of DOM Elements
function GetCellVal(cell){return cell.firstChild.value;}
function GetCellIntVal(cell){return parseInt(GetCellVal(cell));}
function CellGetStr(cell){return cell.innerHTML;}
function CellEleGetVal(cell){if(cell.firstChild.matches('[type="checkbox"]')||cell.firstChild.matches('[type="number"]'))return parseInt(cell.firstChild.value);return cell.firstChild.value;}
//Set Value of DOM Elements
function SetEle(ele,value){if(ele.matches('[type="checkbox"]')){let bit=parseInt(value);ele.value=bit;ele.checked=bit?true:false;}else ele.value=value;}
//Cookies
function GetCookie(cname){
 let name=cname+"=";
 let ca=(decodeURIComponent(document.cookie)).split(';');
 for(let i=0;i<ca.length;i++){
  let c=ca[i];
  while(c.charAt(0)==' '){c=c.substring(1);}
  if(c.indexOf(name)==0)return c.substring(name.length,c.length);}
 return "";}
function GetNrCookie(cname){
 let r=GetCookie(cname);
 if(r=="")r="0";
 return parseInt(r);}
function SetCookie(cname,value){
 document.cookie=cname+"="+value+"; SameSite=Lax";}
//*****************************
//Change Elements attribute
function ElesRemActive(es){
 for(let i=0;i<es.length;++i)es[i].className=es[i].className.replace(" active","");}
function ElesAddActive(e){
 e.className+=" active";}
function ElesDisNone(es){
 for(let i=0;i<es.length;++i)es[i].style.display="none";}
//*****************************
//Tabbed View
function MSelect(nr){
 const nr2Name=["Feedings","Config","ServoSteps","Update"];
 document.getElementById("ResDiv").style.display="none";
 document.getElementById("MainDiv").style.display="block";
 ElesDisNone(document.getElementsByClassName("MenuSelectDiv"));
 ElesRemActive(document.getElementsByClassName("MenuA"));
 ElesAddActive(document.getElementById("M"+nr2Name[nr]));
 document.getElementById(nr2Name[nr]).style.display="block";
 SetCookie("MNr",nr);}
function MRestore(){
 MSelect(GetNrCookie("MNr"));}
//*****************************
var Translations={};
var LangCnt=0;
var LangNr=0;
function SetTrData(data){
 Translations=data;
 LangCnt=Translations["Lang"].length;
 // Create Language-Select Menu
 let langs=Translations["Lang"];
 let t="";
 for(let i=0;i<LangCnt;++i)t+=`<a class="MenuDdA" id="M${langs[i]}" onclick="LSelect(${i})">${langs[i]}</a>`;
 document.getElementsByClassName("MenuDdDiv")[0].innerHTML=t;
 LRestore();}
function LoadTranslations(){GetJData('Translations.json',SetTrData);}
function Tr(name,defStr){
 if(!Translations.hasOwnProperty(name)){
   if(defStr==undefined)
     return name;
   return defStr;}
 let res=Translations[name][LangNr];
 if(res!==undefined)
   return res;
 res=Translations[name][0];
 if(res!==undefined)
   return res;
 return name;}
function TrAll(){
 let es=document.querySelectorAll("span[name]");
 for(let i=0;i<es.length;++i){
  let e=es[i];
  e.innerHTML=Tr(e.attributes["name"].value,e.innerHTML);}}
function LSelect(nr){
 if(nr>=LangCnt)nr=0;
 //Activate Menu-Entry
 let lang=Translations["Lang"][nr];
 ElesRemActive(document.getElementsByClassName("MenuDdA"));
 ElesAddActive(document.getElementById("M"+lang));
 //Change Text of Menu-Entry
 document.getElementById("MLang").innerHTML=lang;
 //Switch LangStyles
 LangNr=nr;
 TrAll();
 SetCookie("LNr",nr);}
function LRestore(){
 LSelect(GetNrCookie("LNr"));}
//*****************************
//Create dynamic Entries
function CD(type,name){
 const type2Func={"t":MkTxt,"T":MkPw,"n":MkNr,"p":MkPin,"P":MkPercent,"u":MkUint,"w":MkMode};
 if(type[0]=="d")return MkRow('<th style="padding-top:15px;"></th>');
 return MkRow(MkCell(name)+MkCell(type2Func[type[0]](),type[1]));}
//*****************************
//Feedings(data)
function MkFeedingsRow(idx,row){
 let res=MkCell(idx)+MkCell(MkCb((row[1]&(1<<7))?1:0))+MkCell(MkTm(row[0]));
 for(let i=0;i<7;++i)res+=MkCell(MkCb((row[1]&(1<<i))?1:0));
 return MkRow(res+MkCell(MkSel(row[2])));}
function SetFeedings(data){
 let res="";for(let r=0;r<data.length;++r)res+=MkFeedingsRow(r,data[r]);
 document.getElementById("FeedData").innerHTML=res;}
function LoadFeedings(){GetJData('FeedTimes.json',SetFeedings);}
//*****************************
//Feedings(submit)
function SubmitFeedings(){
 const bit2Col=[3,4,5,6,7,8,9,1];
 let tab=document.getElementById("FeedData");
 let jdata=[];
 for(let r=0,row;(row=tab.rows[r]);++r){
  let bits=0;for(let b=0;b<8;++b)bits|=GetCellVal(row.cells[bit2Col[b]])<<b;
  jdata.push([GetCellVal(row.cells[2]),bits,GetCellIntVal(row.cells[10])]);}
 PostJRequest(0,"/SetFeedTimes/",jdata);}
//*****************************
//Feedings(Buttons)
function ManualFeed(){
  PostRequest(1,"/ManualFeed/");}
function StopFeed(){
  PostRequest(5,"/StopFeed/");}
//*****************************
//Config(Entries)
function SetConfigEntries(data){
 let res='';for(var key in data)res+=CD(data[key],key); 
 document.getElementById("ConfigData").innerHTML=res;
 GetJData('Config.json',SetConfigData);}
function LoadConfig(){GetJData('ConfigEntries.json',SetConfigEntries);}  
//*****************************
//Config(data)
function SetConfigData(data){
 let tab=document.getElementById("ConfigData");
 for(let r=0,row;(row=tab.rows[r]);++r){
  let key=CellGetStr(row.cells[0]);
  if(key.length){
   SetEle(row.cells[1].firstChild,data[key]);
   if(key=="Hostname")
    ChangeTitle(data[key]);
   }}}
//*****************************
//Config(submit)
function SubmitConfig(){
 let tab=document.getElementById("ConfigData");
 let cfg={};for(let r=0,row;(row=tab.rows[r]);++r){
  let key=CellGetStr(row.cells[0]);
  if(key.length)cfg[key]=CellEleGetVal(row.cells[1]);}
 //Chk Config
 if(cfg["ApPw"].length!=0){//ApPw (no matter whether the AP mode is switched on or off)
   if(cfg["ApPw"].length<8){//ApPw (no matter whether the AP mode is switched on or off)
    alert(Tr("ErrorApPw"));return;}
   if(cfg["ApSsid"].length==0){//ApPw but no ApSsid
    alert(Tr("ErrorApSsid"));return;}}
 if(cfg["WifiMode"]&1){//STA enabled
  if(cfg["WlanSsid"].length==0){//WlanSsid not set
   alert(Tr("ErrorWlanSsid"));return;}
  if(cfg["NtpServerAdr"].length==0){//NtpServerAdr not set
   alert(Tr("ErrorNtpServer"));return;}
  if(cfg["TZ"].length==0){//TZ not set
   alert(Tr("ErrorTZ"));return;}}
 PostJRequest(2,"/SetConfig/",cfg);}
//*****************************
//ServoSteps(data)
function MkServoStepsRow(row){
 return MkRow(MkCell(MkNr(0,180,row[0]))+MkCell(MkNr(20,3000,row[1]))+MkCell(MkNr(0,10000,row[2])));}
function SetServoSteps(data){
 let res="";for(let r=0;r<data.length;++r)res+=MkServoStepsRow(data[r]);
 document.getElementById("ServoStepData").innerHTML=res;}
function LoadServoSteps(){GetJData('ServoSteps.json',SetServoSteps);}
//*****************************
//ServoSteps(submit)
function SubmitServoSteps(){
 let tab=document.getElementById("ServoStepData");
 let jdata=[];
 for(let r=0,row;(row=tab.rows[r]);++r){
  jdata.push([GetCellVal(row.cells[0]),GetCellVal(row.cells[1]),GetCellVal(row.cells[2])]);}
 PostJRequest(4,"/SetServoSteps/",jdata);}
//*****************************
//ServoSteps(Buttons)
function AddServoStep(){
 let res=document.getElementById("ServoStepData").innerHTML;
 res+=MkServoStepsRow([0,0,0]);
 document.getElementById("ServoStepData").innerHTML=res;}
function RemoveServoStep(){
 let tab=document.getElementById("ServoStepData");
 let rowCnt=tab.rows.length;
 tab.deleteRow(rowCnt-1);}
//*****************************
//ResDiv
function ReloadCountdown(){
 var t=10;var x=setInterval(function(){--t;document.getElementById("ReloadTimeOut").innerHTML=t;if(t<=0){clearInterval(x);location.replace("/Index.html");}},1000);
 return `${Tr("ReloadTxt2")}<span id="ReloadTimeOut">10</span>${Tr("ReloadTxt3")}<br><button onclick=location="/Index.html">${Tr("Reload Now")}</button>`;}
function ResHeader(ok,trTxt,fontSize=2){
 const ok2Color=["red","green"];
 return `<font color="${ok2Color[ok]}" style="font-size:${fontSize}em;"><strong>${Tr(trTxt)}</strong></font><br>`;}
function FeedTimesNok(e){e.innerHTML=`${ResHeader(0,"FeedTimesNok")}${ReloadCountdown()}`;}
function FeedTimesOk(e){e.innerHTML=`${ResHeader(1,"FeedTimesOk")}${ReloadCountdown()}`;}
function ConfigNok(e){e.innerHTML=`${ResHeader(0,"ConfigNok")}${ReloadCountdown()}`;}
function ConfigOk(e){e.innerHTML=`${ResHeader(1,"ConfigOk")}${Tr("ReloadTxt1")}<br><button onclick=location="/Index.html">${Tr("Reload")}</button>`;}
function ManualFeedNok(e){e.innerHTML=`${ResHeader(0,"ManualFeedNok")}${ReloadCountdown()}`;}
function ManualFeedOk(e){setTimeout(function(){PostRequest(3,"/MFeedChk/");},1000);e.innerHTML=`${ResHeader(1,"ManualFeedOk",6)}`;}
function StopFeedNok(e){e.innerHTML=`${ResHeader(0,"StopFeedNok")}${ReloadCountdown()}`;}
function StopFeedOk(e){e.innerHTML=`${ResHeader(1,"StopFeedOk",6)}`;}
function MFeedChkActive(e){setTimeout(function(){PostRequest(3,"/MFeedChk/");},1000);}
function MFeedChkDone(e){e.innerHTML+=`<br><button onclick=location="/Index.html">${Tr("ManualFeedDone")}</button>`;}
function ServoStepsNok(e){e.innerHTML=`${ResHeader(0,"ServoStepsNok")}${ReloadCountdown()}`;}
function ServoStepsOk(e){e.innerHTML=`${ResHeader(1,"ServoStepsOk")}${ReloadCountdown()}`;}
function SetStatus(func,ok){
 const funcTab=[[FeedTimesNok,FeedTimesOk],[ManualFeedNok,ManualFeedOk],[ConfigNok,ConfigOk],[MFeedChkActive,MFeedChkDone],[ServoStepsNok,ServoStepsOk],[StopFeedNok,StopFeedOk]];
 let ResDiv=document.getElementById("ResDiv");
 document.getElementById("MainDiv").style.display="none";
 ResDiv.style.display="block";
 let f=funcTab[func][ok];
 if(f) f(ResDiv);}
//*****************************
//Setup
function SetupOnLoaded(){
 LoadTranslations();
 LoadFeedings();
 LoadConfig();
 LoadServoSteps();
 MRestore();
 }
