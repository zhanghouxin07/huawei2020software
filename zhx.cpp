#include <bits/stdc++.h>

using namespace std;

// #define TEST

struct Path{

    int length;
    vector<int> path;

    Path(int length, const vector<int> &path) : length(length), path(path) {}

    bool operator<(const Path&rhs)const{
        if(length!=rhs.length) return length<rhs.length;
        for(int i=0;i<length;i++){
            if(path[i]!=rhs.path[i])
                return path[i]<rhs.path[i];
        }
    }
};

#define DEPTH_HIGH_LIMIT 7
#define DEPTH_LOW_LIMIT 3
class Solution{
public:
    //maxN=560000
    //maxE=280000 ~avgN=28000
    //vector<int> *G;
    vector<vector<int> > G;
    unordered_map<int,int> idHash; //sorted id to 0...n
    vector<string> idsComma; //0...n to sorted id
    vector<string> idsLF; //0...n to sorted id
    vector<int> inputs; //u-v pairs
    vector<int> inDegrees;
    vector<int> outDegrees;
    vector<bool> vis;
    vector<Path> ans[8];
    vector<int> reachable;
    vector<int> s_res;
    vector<unordered_map<int,vector<int>>> P2inv;
    int nodeCnt;
    int ansCnt;

    //TODO:mmap
    void parseInput(string &testFile){
        FILE* file=fopen(testFile.c_str(),"r");
        int u,v,c;
        int cnt=0;
        while(fscanf(file,"%d,%d,%d",&u,&v,&c)!=EOF){
            inputs.push_back(u);
            inputs.push_back(v);
            ++cnt;
        }
#ifdef TEST
        printf("%d Records in Total\n",cnt);
#endif
    }

    void constructGraph(){
        auto tmp=inputs;
        sort(tmp.begin(),tmp.end());
        tmp.erase(unique(tmp.begin(),tmp.end()),tmp.end());
        nodeCnt=tmp.size();
        idsComma.reserve(nodeCnt);
        idsLF.reserve(nodeCnt);
        nodeCnt=0;
        for(int &x:tmp){
            idsComma.push_back(to_string(x)+',');
            idsLF.push_back(to_string(x)+'\n');
            idHash[x]=nodeCnt++;
        }
#ifdef TEST
        printf("%d Nodes in Total\n",nodeCnt);
#endif
        int sz=inputs.size();
        G=vector<vector<int>>(nodeCnt);
        inDegrees=vector<int>(nodeCnt,0);
        outDegrees=vector<int>(nodeCnt,0);
        for(int i=0;i<sz;i+=2){
            int u=idHash[inputs[i]],v=idHash[inputs[i+1]];
            G[u].push_back(v);
            ++inDegrees[v];
            ++outDegrees[u];
        }
    }

    void topoSort(vector<int> &degs,bool doSoring){
        queue<int> que;
        for(int i=0;i<nodeCnt;i++){
            if(0==degs[i])
                que.push(i);
        }
        while(!que.empty()){
            int u=que.front();
            que.pop();
            for(int &v:G[u]) {
                if(0==--degs[v])
                    que.push(v);
            }
        }
        int cnt=0;

        for(int i=0;i<nodeCnt;i++){
            if(degs[i]==0){
                G[i].clear();
                cnt++;
            }else if(doSoring){
                sort(G[i].begin(),G[i].end());
            }
        }
        
#ifdef TEST
        printf("%d Nodes eliminated in TopoSort\n",cnt);
#endif
    }

    void constructP2(){
        P2inv=vector<unordered_map<int,vector<int>>>(nodeCnt,unordered_map<int,vector<int>>());
        for(int i=0;i<nodeCnt;i++){
            auto &gi=G[i];
            for(int &k:gi){
                auto &gk=G[k];
                for(int &j:gk){
                    if(j!=i){
                        P2inv[j][i].push_back(k);
                    }
                }
            }

        }
        for(int i=0;i<nodeCnt;i++){
            for(auto &x:P2inv[i]){
                if(x.second.size()>1){
                    sort(x.second.begin(),x.second.end());
                }
            }
        }
    }

    void dfs(int head,int cur,int depth,vector<int> &path){
        vis[cur]=true;
        path.push_back(cur);
        auto &gCur=G[cur];
        auto it=lower_bound(gCur.begin(),gCur.end(),head);
        //handle [3,6]
        if(it!=gCur.end() && *it==head && depth>=DEPTH_LOW_LIMIT && depth<DEPTH_HIGH_LIMIT) {
            ans[depth].emplace_back(Path(depth,path));
            ++ansCnt;
        }

        if(depth<DEPTH_HIGH_LIMIT-1){
            for(;it!=gCur.end();++it){
                if(!vis[*it]){
                    dfs(head,*it,depth+1,path);
                }
            }
        }else if(reachable[cur]>-1 && depth==DEPTH_HIGH_LIMIT-1){ //handle [7]
            auto ks=P2inv[head][cur];
            int sz=ks.size();
            for(int idx=reachable[cur];idx<sz;++idx){
                int k=ks[idx];
                if(vis[k]) continue;
                auto tmp=path;
                tmp.push_back(k);
                ans[depth+1].emplace_back(Path(depth+1,tmp));
                ++ansCnt;
            }
        }
        vis[cur]=false;
        path.pop_back();
    }
    
    //
    void find_k(bool * vis, int head, int cur){
		if(P2inv[head].count(cur)){
			vector<int> vec = P2inv[head][cur];
			int sz=vec.size(),k;
			for(int idx=0;idx<sz;++idx){
				k=vec[idx];
				if(!vis[k] && k>head){
					s_res.emplace_back(k);
                    int n=s_res.size();
                    ans[n].emplace_back(Path(n,s_res));
					s_res.pop_back();	
				}
			}
		}
	}

    void Recurrence_map(){
        constructP2();
        int ID_num = nodeCnt;
		bool vis[ID_num] = {0};
		for(auto i=0 ;i<ID_num; i++){
#ifdef TEST
            if(i%500==0){
                cout<<i<<"/"<<ID_num<<endl;
                int _ansCnt=0;
        		for(int i=DEPTH_LOW_LIMIT;i<=DEPTH_HIGH_LIMIT;i++) {
            	_ansCnt+=ans[i].size();
        }
            }
#endif
			if(G[i].size()!=0){
				vis[i]=1; s_res.emplace_back(i); // push 1
				for(auto j=0; j<G[i].size(); j++){
					int tj = G[i][j];
					if(tj>i&&!vis[tj]){
						vis[tj]=1; s_res.emplace_back(tj); //push 2
						for(auto k=0; k<G[tj].size(); k++){
							int tk = G[tj][k];
							if(tk>i&&!vis[tk]){
								vis[tk]=1; s_res.emplace_back(tk); //push 3
								for(auto h=0; h<G[tk].size();h++){
									int th = G[tk][h];
									if(th==i)  ans[3].emplace_back(Path(3,s_res));// find 3
									if(th>i&&!vis[th]){
										vis[th]=1; s_res.emplace_back(th); //push 4
										for( auto l=0;l<G[th].size();l++){
											int tl=G[th][l];
											if(tl==i)  ans[4].emplace_back(Path(4,s_res));// find 4
											if(tl>i&&!vis[tl]){
												vis[tl]=1; s_res.emplace_back(tl); //push 5
												//find 6
												find_k(vis, i, tl);
												for(auto n=0;n<G[tl].size();n++){
													int tn=G[tl][n];
													if(tn==i)  ans[5].emplace_back(Path(5,s_res));;//  find 5
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
		// if(print)cout<<"length of p1 = "<<p1.size()<<endl;
	}
    //search from 0...n
    void solve(){
        ansCnt=0;
        constructP2();
        vis=vector<bool>(nodeCnt,false);
        vector<int> path;
        reachable=vector<int>(nodeCnt,-1);
        vector<int> currentJs(nodeCnt);
        for(int i=0;i<nodeCnt;++i){
#ifdef TEST
            if(i%100==0){
                cout<<i<<"/"<<nodeCnt<<" ~ "<<ansCnt<<endl;
            }
#endif
            if(!G[i].empty()){
                for(auto &js:P2inv[i]){
                    int j=js.first;
                    if(j>i){
                        auto &val=js.second;
                        int sz=val.size();
                        int lb=lower_bound(val.begin(),val.end(),i)-val.begin();
                        if(lb<val.size()) reachable[j]=lb;
                        currentJs.push_back(j);
                    }
                }
                dfs(i,i,1,path);
                for(int &x:currentJs)
                    reachable[x]=-1;
                currentJs.clear();
            }
        }

#ifdef TEST
        for(int i=DEPTH_LOW_LIMIT;i<=DEPTH_HIGH_LIMIT;i++){
            printf("Loop Size %d: %d/%d ~ %.3lf\n",i,ans[i].size(),ansCnt,ans[i].size()*1.0/ansCnt);
        }
#endif
    }

    void save(const string &outputFile){
        auto t=clock();
        ofstream out;
        out.open(outputFile);

#ifdef TEST
        printf("Total Loops %d\n",ansCnt);
#endif
        out<<ansCnt<<"\n";
        for(int i=DEPTH_LOW_LIMIT;i<=DEPTH_HIGH_LIMIT;i++) {
            for (auto &x:ans[i]) {
                auto path = x.path;
                int sz = path.size();
                for(int j=0;j<sz-1;j++)
                    out<<idsComma[path[j]];
                out<<idsLF[path[sz-1]];
            }
        }
        cout<<clock()-t<<endl;
    }

    void save_fputs(const string &outputFile){
        auto t=clock();
        FILE *fp = fopen(outputFile.c_str(), "w");
        char buf[1024];

#ifdef TEST
        printf("Total Loops %d\n",ansCnt);
#endif
        int idx=sprintf(buf,"%d\n",ansCnt);
        buf[idx]='\0';
        fputs(buf, fp);
        for(int i=DEPTH_LOW_LIMIT;i<=DEPTH_HIGH_LIMIT;i++) {
            //sort(ans[i].begin(),ans[i].end());
            for (auto &x:ans[i]) {
                auto path = x.path;
                int sz = path.size();
                idx=0;
                for(int j=0;j<sz-1;j++){
                    idx+=sprintf(buf+idx,"%s",idsComma[path[j]].c_str());
                }
                idx+=sprintf(buf+idx,"%s",idsLF[path[sz-1]].c_str());
                buf[idx]='\0';
                fputs(buf, fp);
            }
        }
        fclose(fp);
        cout<<clock()-t<<endl;
    }

    void save_fwrite(const string &outputFile){
        auto t=clock();
        FILE *fp = fopen(outputFile.c_str(), "wb");
        char buf[1024];
        ansCnt=0;
        for(int i=DEPTH_LOW_LIMIT;i<=DEPTH_HIGH_LIMIT;i++) {
            ansCnt+=ans[i].size();
        }
#ifdef TEST
        printf("Total Loops %d\n",ansCnt);
#endif
        int idx=sprintf(buf,"%d\n",ansCnt);
        buf[idx]='\0';
        fwrite(buf, idx , sizeof(char), fp );
        for(int i=DEPTH_LOW_LIMIT;i<=DEPTH_HIGH_LIMIT;i++) {
            //sort(ans[i].begin(),ans[i].end());
            for (auto &x:ans[i]) {
                auto path = x.path;
                int sz = path.size();
                //idx=0;
                for(int j=0;j<sz-1;j++){
                    auto res=idsComma[path[j]];
                    fwrite(res.c_str(), res.size() , sizeof(char), fp );
                }
                auto res=idsLF[path[sz-1]];
                fwrite(res.c_str(), res.size() , sizeof(char), fp );
            }
        }
        fclose(fp);
        cout<<(float)(clock()-t)/CLOCKS_PER_SEC<<endl;
    }
};

int main()
{
    //54.3738.58284.77409.1004812.2755223.2861665.2896262.3512444
    string testFile = "/data/test_data.txt";
	string outputFile = "/projects/student/result.txt";
//     string testFile = "D:/recentwork/huawei2020/test_data/2020HuaweiCodecraft-TestData-master/1004812/test_data.txt";
//     string outputFile = "output.txt";
#ifdef TEST
    string answerFile = "result.txt";
#endif
    auto t=clock();
//    for(int i=0;i<100;i++){
    Solution solution;
    solution.parseInput(testFile);
    solution.constructGraph();
    solution.topoSort(solution.inDegrees,false);
    solution.topoSort(solution.outDegrees,true);
    // solution.solve();
    solution.Recurrence_map();
    solution.save_fwrite(outputFile);

    cout<<"running time = "<<(double)(clock()-t)/CLOCKS_PER_SEC<<endl;
//    }

    return 0;
}
