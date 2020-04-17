#include <bits/stdc++.h>

using namespace std;

#define print true
// #define print false

struct path{
	vector<int> node;
	int len;

	path(const vector<int> &node, int len) :  node(node),len(len) {}
	bool operator<(const path & tmp)const{
        if(len!=tmp.len) return len<tmp.len;
        for(int i=0;i<len;i++){
            if(node[i]!=tmp.node[i])
                return node[i]<tmp.node[i];
        }
    }
};

class transfer{
	public:
		vector<vector<int>> Graph,R_Graph;
		vector<int> Readin;
		set<int> ID;
		unordered_map<int,int> map;
		unordered_map<int,string> remap_str,remap_ent;
		int ID_num;
		vector<path> p,p1;
		vector<unordered_map<int,vector<int> > > P2inv;
		vector<int> s_res;
		 
		
	void readfile(string * filePath){
		FILE* file=fopen(filePath->c_str(),"r");
        int send,rece,money;
        int infor_cnt=0;
        while(fscanf(file,"%d,%d,%d",&send,&rece,&money)!=EOF){
            Readin.emplace_back(send);
            Readin.emplace_back(rece);
            Readin.emplace_back(money);
            ID.insert(send);
            ID.insert(rece);
            infor_cnt++;
        }
        if(print) cout<<"transfer_num = "<<infor_cnt<<endl;
	}
	void build_Adjacency_graph(){
		// ID map
		ID_num = ID.size();
		int map_cnt=0;
		for(auto x=ID.begin(); x!= ID.end();x++){
			map[*x] = map_cnt;
			remap_str[map_cnt] = to_string(*x)+",";
			remap_ent[map_cnt] = to_string(*x)+"\n";
			map_cnt++;
		} 
		// build Adjacency List
		Graph = vector<vector<int>>(ID_num);
		R_Graph = vector<vector<int>>(ID_num);
		for(int i=0; i<Readin.size(); i+=3){
			Graph[map[Readin[i]]].emplace_back(map[Readin[i+1]]);
			R_Graph[map[Readin[i+1]]].emplace_back(map[Readin[i]]);
		}
		//P2inv
		P2inv=vector<unordered_map<int,vector<int>>>(ID_num,unordered_map<int,vector<int>>());
		for(int i=0;i<ID_num;i++){
            auto &gi=Graph[i];
            for(int &k:gi){
                auto &gk=Graph[k];
                for(int &j:gk){
                    if(j!=i){
                        P2inv[j][i].emplace_back(k);
                    }
                }
            }

        }
        for(int i=0;i<ID_num;i++){
            for(auto &x:P2inv[i]){
                if(x.second.size()>1){
                    sort(x.second.begin(),x.second.end());
                }
            }
        }
		if(print) cout<<"map_Cnt = "<<map_cnt<<endl;
	}
	
	int find_k(bool * vis, int head, int cur){
		if(P2inv[head].count(cur)){
			vector<int> vec = P2inv[head][cur]; 
			int sz=vec.size(),k;
			for(int idx=0;idx<sz;++idx){
				k=vec[idx];
				if(!vis[k] && k>head){
					s_res.emplace_back(k);
					p1.emplace_back(path(s_res,(int)s_res.size()));
					s_res.pop_back();	
				}
			}
		}
		return 0;
	}
	void reverse_Recurrence_map(){
		if(print)cout<<"3+3 Recurrence_map"<<endl;
		bool vis[ID_num] = {0};
		
	}
	void dfs(bool * _vis, int head, int cur, int depth){
		s_res.emplace_back(cur);_vis[cur]=1;
		if(depth<6)
		for(auto i=0; i<Graph[cur].size(); i++){
			int ti = Graph[cur][i];
			if(ti == head && depth>=3) {
				p1.emplace_back(path(s_res,(int)s_res.size()));
				continue;
			}
			if(ti>head && _vis[ti]==0)
				dfs(_vis, head, ti, depth+1);
		}
		if(depth==5)find_k(_vis,head,cur);  //find 6
		if(depth==6)find_k(_vis,head,cur);	//find 7
		s_res.pop_back();_vis[cur]=0;
	}
	void dfs_main(){
		bool vis[ID_num] = {0};
		for(int i=0; i<ID_num;i++){
			if(print) if(i%500 == 0) cout<<"i "<<i<<" percent : "<<(float) i/ID_num<<" Recurrence_res size : "<<p1.size()<<endl;
			if(Graph[i].size()!=0)
				dfs(vis, i, i, 1);
		}
	}
	void Recurrence_map(){
		if(print)cout<<"Recurrence_map"<<endl;
		bool vis[ID_num] = {0};
		for(auto i=0 ;i<ID_num; i++){
			if(Graph[i].size()!=0){
				if(print) if(i%500 == 0) cout<<"i "<<i<<" percent : "<<(float) i/ID_num<<" Recurrence_res size : "<<p1.size()<<endl; 
				vis[i]=1; s_res.emplace_back(i); // push 1
				for(auto j=0; j<Graph[i].size(); j++){
					int tj = Graph[i][j];
					if(tj>i&&!vis[tj]){
						vis[tj]=1; s_res.emplace_back(tj); //push 2
						for(auto k=0; k<Graph[tj].size(); k++){
							int tk = Graph[tj][k];
							if(tk>i&&!vis[tk]){
								vis[tk]=1; s_res.emplace_back(tk); //push 3
								for(auto h=0; h<Graph[tk].size();h++){
									int th = Graph[tk][h];
									if(th==i)  p1.emplace_back(path(s_res,(int)s_res.size()));// find 3
									if(th>i&&!vis[th]){
										vis[th]=1; s_res.emplace_back(th); //push 4
										for( auto l=0;l<Graph[th].size();l++){
											int tl=Graph[th][l];
											if(tl==i)  p1.emplace_back(path(s_res,(int)s_res.size()));// find 4
											if(tl>i&&!vis[tl]){
												vis[tl]=1; s_res.emplace_back(tl); //push 5
												//find 6
												find_k(vis, i, tl);
												for(auto n=0;n<Graph[tl].size();n++){
													int tn=Graph[tl][n];
													if(tn==i)  p1.emplace_back(path(s_res,(int)s_res.size()));//  find 5
													if(tn>i&&!vis[tn]){
														vis[tn]=1; s_res.emplace_back(tn); //push 6
														//find 7
														find_k(vis, i, tn);
														//end
														vis[tn]=0; s_res.pop_back();
													}
												}
												vis[tl]=0; s_res.pop_back(); 
											}
										}
										vis[th]=0; s_res.pop_back(); 
									}
								}
								vis[tk]=0; s_res.pop_back(); 
							}
						}
						vis[tj]=0;s_res.pop_back();
					}
				}
				vis[i]=0; s_res.pop_back();
			}
		}
		//print path
		// for(auto &pi: p1){
		// 	auto pii = pi.node;
		// 	for(auto &pj: pii ) cout<<pj<<' '; cout<<endl;
		// }
		if(print)cout<<"length of p1 = "<<p1.size()<<endl;
	}

	void writefile(const string *outputFile){
		auto t=clock();		
		sort(p1.begin(),p1.end());
		FILE *out = fopen(outputFile->c_str(),"w");
		char buf[1024];
		int idx=sprintf(buf,"%d\n",(int)p1.size());
        buf[idx]='\0';
        fwrite(buf, idx , sizeof(char), out );
		for(auto &pi: p1){
			int lpi = pi.node.size();
		 	for(int j=0;j<lpi-1;j++){
				 auto res = remap_str[pi.node[j]];
				 fwrite(res.c_str(), res.size() , sizeof(char), out );
			 }
			 auto res=remap_ent[pi.node[lpi-1]];
			fwrite(res.c_str(), res.size() , sizeof(char), out );
		}
		fclose(out);

		if(print)cout<<"io time = "<<(float)(clock()-t)/CLOCKS_PER_SEC<<endl;
	}
}; 
int main(){
	auto st=clock();
	transfer transfer;
	//54.3738.58284.77409.1004812.2755223.2861665.2896262.3512444
	string filepath = "/home/lift/Downloads/huawei2020/test_data/2020HuaweiCodecraft-TestData-master/3512444/test_data.txt";
	string resfilepath = "hx_output.txt";

	//  string filepath = "/data/test_data.txt";
	//  string resfilepath = "/projects/student/result.txt";
	transfer.readfile(&filepath);
	transfer.build_Adjacency_graph();

	// transfer.Recurrence_map();		
	transfer.dfs_main();
	transfer.writefile(&resfilepath);
	if(print) cout<<"running time = "<<(float)(clock()-st)/CLOCKS_PER_SEC<<endl;
	return 0;
}
