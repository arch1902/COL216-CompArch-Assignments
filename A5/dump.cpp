       bool flag = false;
        pair<int,string> removed_register;
        //DRAM 
        if (!Dram_queue.empty()){
            vector<string> Curr_executed = Dram_queue[0];
            int address = stoi(Curr_executed[1]);
            int row = (address+CORE_MEMORY*stoi(Curr_executed[6]))/1024;
            int  col = (address+CORE_MEMORY*stoi(Curr_executed[6]))%1024;
            if(address >= CORE_MEMORY|| col%4 !=0){
                cout<<"Program tried to access invalid memory address "<<address<<endl;
                exit(-1);
            }
            // for(auto k: Dram_queue[0]){
            //      out<<k<<" ";
            // }
            // out<<endl;
            while(true){
                //out<<Dram_queue[0][3]<<endl;0
                if (Dram_queue[0][3]=="0"){
                    out<<"File Number "<<stoi(Dram_queue[0][6])+1<<" : Started "<<Dram_queue[0][0]<<" "<<Dram_queue[0][1]<<" "<<Dram_queue[0][2]<<" on Line "<<Dram_queue[0][5]<<endl;
                    int curr = 0;
                    if(Dram_queue[0][0]=="sw") row_buffer_updates++;
                    if(last_buffer_row!=row){
                        curr += ROW_ACCESS_DELAY;
                        row_buffer_updates++;
                        if (last_buffer_row!=-1){
                            if(dirty_bit){
                            curr += ROW_ACCESS_DELAY;
                                out<<"Row "<<last_buffer_row<<" will be copied back to DRAM and ";
                            }else{
                                out<<"Row "<<last_buffer_row<<" need NOT be copied back to DRAM and "; 
                            }
                        }
                        out<<"Row "<<row<<" will be activated\n";
                        dirty_bit = 0;
                    }
                    curr += COL_ACCESS_DELAY;
                    Dram_queue[0][4] = to_string(curr);
                    Dram_queue[0][3] = "1";
                    out<<"File Number "<<stoi(Dram_queue[0][6])+1<<" : Completed "<<"1"<<"/"<<Dram_queue[0][4]<<endl;
                    break;
                }
                out<<"File Number "<<stoi(Dram_queue[0][6])+1<<" : Completed "<<to_string(stoi(Dram_queue[0][3])+1)<<"/"<<Dram_queue[0][4]<<endl;
                if (stoi(Dram_queue[0][3])==ROW_ACCESS_DELAY-1){
                    if(Dram_queue[0][4]==to_string(2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
                        for (int j = 0;j<256;j++){
                            if (Dram_Memory[last_buffer_row][j] != Row_buffer[j]){
                                out<<"Memory at "<<last_buffer_row*1024+j*4<<" = "<<to_string(Row_buffer[j])<<endl;
                            }
                        }
                        Dram_Memory[last_buffer_row] = Row_buffer;
                    }
                    if(stoi(Dram_queue[0][4])==ROW_ACCESS_DELAY+COL_ACCESS_DELAY){
                        Row_buffer = Dram_Memory[row];
                        last_buffer_row = row;
                    }
                    Dram_queue[0][3] = to_string(stoi(Curr_executed[3])+1);
                    break;
                }
                if (stoi(Dram_queue[0][3])==2*ROW_ACCESS_DELAY-1 && Dram_queue[0][4]==to_string(2*ROW_ACCESS_DELAY+COL_ACCESS_DELAY)){
                    Row_buffer = Dram_Memory[row];
                    last_buffer_row = row;
                    Dram_queue[0][3] = to_string(stoi(Curr_executed[3])+1);
                    break;
                }

                if(stoi(Dram_queue[0][3])==stoi(Dram_queue[0][4])-1){
                    total_int_executed++;
                    if(Dram_queue[0][0]=="lw"){
                        all_regs[stoi(Dram_queue[0][6])][Dram_queue[0][2]] = Row_buffer[col/4];
                        flag  = true;
                        removed_register = {stoi(Dram_queue[0][6]),Dram_queue[0][2]};
                        out<<"File Number "<<stoi(Dram_queue[0][6])+1<<" : "<<removed_register.second<<" = "<<Row_buffer[col/4]<<endl;
                        //Blocking_registers[Dram_queue[0][2]] -=1;
                    }
                    if(Dram_queue[0][0] =="sw"){
                        Row_buffer[col/4] = stoi(Dram_queue[0][2]);
                        dirty_bit = 1;
                    }
                    out<<"File Number "<<stoi(Dram_queue[0][6])+1<<" : Finished Instruction "<<Dram_queue[0][0]<<" "<<Dram_queue[0][1]<<" "<<Dram_queue[0][2]<<" on Line "<<Dram_queue[0][5]<<endl;
                    Dram_queue.erase(Dram_queue.begin());
                    sort_queue();
                    //Dram_queue[0][3] = to_string(stoi(Dram_queue[0][3])+1);
                    break;
                }
                Dram_queue[0][3] = to_string(stoi(Dram_queue[0][3])+1);
                break;
            }
        }


            if (pushed_from_core == executing_core){
                while(true){
                    if (Dram_queue[pushed_from_core].back()[7] == "0"){
                        int x = Dram_queue[pushed_from_core].size();
                        int firstrow = stoi(Dram_queue[pushed_from_core][0][1])/1024;
                        int thisrow = stoi(Dram_queue[pushed_from_core].back()[1])/1024;
                        if (firstrow != thisrow){request_issued = false;break;}
                        Dram_queue[pushed_from_core].back()[7] == to_string(x-1);
                        break;
                    }else{
                        int pos = stoi(Dram_queue[pushed_from_core].back()[7]);
                        int prev_row = stoi(Dram_queue[pushed_from_core][pos-1][1])/1024;
                        int curr_row = stoi(Dram_queue[pushed_from_core][pos][1])/1024;
                        if(curr_row==prev_row){request_issued = false;break;}

                    }
                }
            }