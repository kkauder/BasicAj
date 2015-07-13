
foreach In  ( AjResults/Pt1_Tow*ppInAuAuAj_*.root )
    set Out  = `echo $In | sed s/Pt1_Tow/Tow/g`
    mv -v $In $Out
end
