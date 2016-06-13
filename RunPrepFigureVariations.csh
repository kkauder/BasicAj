#!/usr/bin/env csh

set files = ( "AjResults/Tow0_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root" )
set files = ( $files AjResults/Tow0_Eff-1_Fresh_NicksList_HC100_ppInAuAuAj.root )
set files = ( $files AjResults/Tow0_Eff1_Fresh_NicksList_HC100_ppInAuAuAj.root )
set files = ( $files AjResults/Tow-1_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root )
set files = ( $files AjResults/Tow1_Eff0_Fresh_NicksList_HC100_ppInAuAuAj.root )
set files = ( $files AjResults/Fresh_NicksList_HC100_AuAu.root )

foreach file ( $files )
    root -l -b -q PrepFigureVariations.cxx\(\"$file\"\) 
end

# 




