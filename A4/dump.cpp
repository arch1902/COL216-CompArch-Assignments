void writeback(int r){
    for(int i =0;i<256;i++){
        if(Dram_Memory[r+511][i]!=Row_buffer[i]){
            out<<"Memory at "<<(r+511)*1024+i*4<<" = "<<Row_buffer[i]<<endl;
        }
        Dram_Memory[r+511][i]=Row_buffer[i];
        data_memory[(r+511)*1024+i*4] = (Row_buffer[i]== "")? 0: stoi(Row_buffer[i]);
    }
    return;
}

void DRAM_activity_manager(int count, int expectation, int row, int last_row, int col){
    if (expectation == 2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
        if(count<ROW_ACCESS_DELAY){
            out<<"Write back to row "<<last_row<<" Completed "<<count<<"/"<<ROW_ACCESS_DELAY<<endl;
        }else if(count == ROW_ACCESS_DELAY){
            out<<"Write back to row "<<last_row<<" Completed "<<count<<"/"<<ROW_ACCESS_DELAY<<endl;
            writeback(last_row);
        }
        else if(count<2*ROW_ACCESS_DELAY){
            out<<"Activating row "<<row<<" Completed "<<count-ROW_ACCESS_DELAY<<"/"<<ROW_ACCESS_DELAY<<endl;
        }else if(count==2*ROW_ACCESS_DELAY){
            Row_buffer = Dram_Memory[511+row];
            out<<"Activating row "<<row<<" Completed "<<count-ROW_ACCESS_DELAY<<"/"<<ROW_ACCESS_DELAY<<endl;
        }else if(count < 2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
            out<<"Accessing column "<<col<<" Completed "<<count-2*ROW_ACCESS_DELAY<<"/"<<COL_ACCESS_DELAY<<endl;
        }else{
            out<<"Accessing column "<<col<<" Completed "<<count-2*ROW_ACCESS_DELAY<<"/"<<COL_ACCESS_DELAY<<endl;
            out<<"Completed DRAM accsess!!"<<endl;
        }
    }else if(expectation == ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
        if(count<ROW_ACCESS_DELAY){
            out<<"Activating row "<<row<<" Completed "<<count<<"/"<<ROW_ACCESS_DELAY<<endl;
        }else if(count==ROW_ACCESS_DELAY){
            Row_buffer = Dram_Memory[511+row];
            out<<"Activating row "<<row<<" Completed "<<count<<"/"<<ROW_ACCESS_DELAY<<endl;
        }else if(count < ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
            out<<"Accessing column "<<col<<" Completed "<<count-ROW_ACCESS_DELAY<<"/"<<COL_ACCESS_DELAY<<endl;
        }else{
            out<<"Accessing column "<<col<<" Completed "<<count-ROW_ACCESS_DELAY<<"/"<<COL_ACCESS_DELAY<<endl;
            out<<"Completed DRAM accsess!!"<<endl;
        }
    }else{
        if(count < COL_ACCESS_DELAY){
            out<<"Accessing column "<<col<<" Completed "<<count<<"/"<<COL_ACCESS_DELAY<<endl;
        }else{
            out<<"Accessing column "<<col<<" Completed "<<count<<"/"<<COL_ACCESS_DELAY<<endl;
            out<<"Completed DRAM accsess!!"<<endl;
        }
    }
    return;
}