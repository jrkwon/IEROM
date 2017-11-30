#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QDataStream>
#include <QPair>
#include <qmath.h>

#include "itkImage.h"
#include "itkSphereSpatialFunction.h"

#define THRESHOLD 20
#define SPHERE_RADIUS 16

 typedef struct {
    int  node_num;
    int type; // 0-Undefined, 1-Soma, 2-Axon, 3-Dendrite, 4-Apical_dendrite, 5-Fork_point, 6-End_point, 7-Custom
    double pos_x;
    double pos_y;
    double pos_z;
    double radius;
    int parent;
} vessel_node;

typedef struct{
  int Vol_ID[3];
  int x_dim;
  int y_dim;
  int z_dim;
  QVector<vessel_node>  vessels;
  QVector<vessel_node> terminals_xh;
  QVector<vessel_node> terminals_yh;
  QVector<vessel_node> terminals_zh;
  QVector<vessel_node> terminals_xl;
  QVector<vessel_node> terminals_yl;
  QVector<vessel_node> terminals_zl;
} VesselBlock;

typedef itk::Vector<double, 3> VectorType;
typedef itk::SphereSpatialFunction<3> SphereType;
typedef SphereType::InputType SpherePointPosType;

void read_swc_files(QVector<VesselBlock>* vessel_blocks,QString* Path);
void stitch(VesselBlock* vol_fixed, VesselBlock*vol_toappend,int direction,QString* Path);//, double size_in_dir);
void write_node_pairs(QList < QPair<int,int> > node_pairs);
void write_stitched_swc(VesselBlock* volume, std::string path);
QList < QPair<int,int> > get_node_pairs(QVector<vessel_node>* high_nodes, QVector<vessel_node>*low_nodes, int x_offset, int y_offset, int z_offset);
QString display(vessel_node* node);
QVector< VesselBlock> vessel_blocks;

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  QString Path = "C:/Users/BI2S_STUDENT/Google Drive/NeuronStitching/Data/Prof Kwon Data/swc";
  //QString Path = "C:/Users/BI2S_STUDENT/Google Drive/NeuronStitching/Data/Validation2";
  QString newPath = Path;
  read_swc_files(&vessel_blocks,&Path);
  int iter_max = int(log2(vessel_blocks.size()));

  for(int iter_count = 0; iter_count < iter_max; iter_count++) {
      read_swc_files(&vessel_blocks,&newPath);
      newPath = Path + "/iter_" + QString::number(iter_count);
      QDir dir(newPath);
      if(!dir.exists()) dir.mkpath(newPath);

  //Now I have all the swc files as structures
  //if(vessel_blocks.size() % 2 == 0) stitchn_even(&vessel_blocks);
  //else stitchn_odd(&vessel_blocks);
  //Now that all the blocks are read, stitch them, 2 at a time.
  //I have to stitch based on VolumeID. So check which digit is different (only one of them should be different)
  //stich_dir = 0 (invalid), 1(x), 2(y), 3(z)
  int stitch_dir = 0;
  QList<int> stitched;
  for(int block1_num=0;block1_num < vessel_blocks.size();block1_num++) {
      if(stitched.indexOf(block1_num) == -1){
          for(int block2_num=0;block2_num < vessel_blocks.size();block2_num++) {
              if(stitched.indexOf(block2_num) == -1){
                  bool x_match = (vessel_blocks[block1_num].Vol_ID[0] == vessel_blocks[block2_num].Vol_ID[0]);
                  bool y_match = (vessel_blocks[block1_num].Vol_ID[1] == vessel_blocks[block2_num].Vol_ID[1]);
                  bool z_match = (vessel_blocks[block1_num].Vol_ID[2] == vessel_blocks[block2_num].Vol_ID[2]);
                  bool stitch_decision = (x_match^y_match^z_match == 0) &&  (x_match|y_match|z_match == 1);
                  if(stitch_decision) {  //Assume atleast one block will get picked to stitch, else something wrong with file naming!
                      qDebug() << " I can stitch these volumes now!";
                      stitched.append(block1_num);
                      stitched.append(block2_num);
                      if(x_match == 0) stitch_dir = 1;
                      else if(y_match == 0) stitch_dir = 2;
                      else stitch_dir = 3;
                      stitch(&vessel_blocks[block1_num],&vessel_blocks[block2_num],stitch_dir,&newPath);//,213); //The fourth input is not reqd, take from second block in stitch function
                      break;
                    }
                }
            }
        }
    }
  stitched.clear();
  vessel_blocks.clear();
  qDebug() << "Completed one loop iteration";
    }

  qDebug() << "DONE!";
  return a.exec();
}

//+++++++++++++++++++++++++++++++//
//Read all the vessels into structures.
//+++++++++++++++++++++++++++++++//
void read_swc_files(QVector<VesselBlock> *vessel_blocks,QString* Path) {
  QDirIterator dir_iter ( *Path, QStringList()  << "*.swc", QDir::Files);
  while (dir_iter.hasNext()){

      QString next = dir_iter.next();
      QString inFileName = dir_iter.fileInfo().baseName();
      QStringList list = inFileName.split("_");

      VesselBlock vblock;
      vblock.Vol_ID[0] = list[list.size() - 3].toInt(); //x
      vblock.Vol_ID[1] = list[list.size() - 2].toInt(); //y
      vblock.Vol_ID[2] = list[list.size() - 1].toInt(); //z
      //qDebug() << vessel.Vol_ID[0] << vessel.Vol_ID[1] << vessel.Vol_ID[2];
      QFile file(next);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
          qDebug() << "Could not open the file in RW mode";
          return;
        }
      QTextStream in(&file);

      QVector<vessel_node>  node_list(0);
      QVector<int> node_num_list(0);
      QVector<int> parent_list(0);
      QVector<vessel_node> terminals(0);

      while(!in.atEnd()){
          vessel_node node;
          QString line = in.readLine();
          QStringList somelist = line.split(' ');
          if(line[0] != '#'){
              //qDebug() << somelist;
              node.node_num = somelist[0].toInt();
              node.type = somelist[1].toInt();
              node.pos_x = somelist[2].toDouble();
              node.pos_y = somelist[3].toDouble();
              node.pos_z = somelist[4].toDouble();
              node.radius = somelist[5].toDouble();
              node.parent = somelist[6].toInt();
              node_list.append(node);
             }
          else if(somelist.size() > 1){
              if(somelist[0] == "#XYZ"){
                  vblock.x_dim = somelist[1].toInt();
                  vblock.y_dim = somelist[2].toInt();
                  vblock.z_dim = somelist[3].toInt();
                }
            }
        }
      file.close();

      vblock.vessels = node_list;

      for (int i = 0; i < node_list.size(); i++){
          node_num_list.append(node_list.at(i).node_num);
          parent_list.append(node_list.at(i).parent);
        }

      for (int i = 0; i < parent_list.size(); i++){
           // Nodes without a parent, root/seed nodes near surface
          if(parent_list.at(i) == -1) terminals.append(node_list.at(i));
          //Nodes without a child
          else if(!parent_list.contains(node_num_list.at(i))) terminals.append(node_list.at(i));
        }

      //Find the terminals in x,y,z directions (+ and - => 6 faces)
      //How do we optimize this? I can include the meta-info in the swc file in my own tracing algorithm
      //MAKE THIS A LOT BETTER> THIS IS TOO MUCH REDUNDANT INFORMATION
      for (int i = 0; i < terminals.size(); i++){
          if(terminals.at(i).pos_x > vblock.x_dim - THRESHOLD){
              vblock.terminals_xh.append(terminals.at(i));
            }
          if(terminals.at(i).pos_y > vblock.y_dim - THRESHOLD) {
              vblock.terminals_yh.append(terminals.at(i));
            }
          if(terminals.at(i).pos_z > vblock.z_dim - THRESHOLD) {
              vblock.terminals_zh.append(terminals.at(i));
            }
          if(terminals.at(i).pos_x < THRESHOLD) {
              vblock.terminals_xl.append(terminals.at(i));
            }
          if(terminals.at(i).pos_y < THRESHOLD) {
              vblock.terminals_yl.append(terminals.at(i));
            }
          if(terminals.at(i).pos_z < THRESHOLD) {
              vblock.terminals_zl.append(terminals.at(i));
            }
        }

      vessel_blocks->append(vblock);
      node_list.clear();
      node_num_list.clear();
      parent_list.clear();
      terminals.clear();
    }
}

//----------------------//
//display(&result);   //
//----------------------//
QString display(vessel_node* node){
  QString msg;
  msg  = QString::number(node->node_num) + " ";
  msg  += QString::number(node->type) + " ";
  msg  += QString::number(node->pos_x) + " ";
  msg  += QString::number(node->pos_y) + " ";
  msg  += QString::number(node->pos_z)  + " ";
  msg  += QString::number(node->radius) + " ";
  msg  += QString::number(node->parent);
  return msg;
}

//-------------------------------------------------------------------------------------//
// FUNCTION : GET NODE PAIRS FROM TWO LISTS OF TERMINALS  //
//------------------------------------------------------------------------------------//
QList <QPair<int,int> > get_node_pairs(QVector<vessel_node>* high_nodes, QVector<vessel_node>*low_nodes, int x_offset, int y_offset, int z_offset) {

  QList <QPair<int,int> > node_pairs;
  foreach(vessel_node vessel,*high_nodes){
      SphereType::Pointer spherefunc = SphereType::New();
      SpherePointPosType center;
      double sphere_radius;
      center[0] = (double)(vessel.pos_x - x_offset);
      center[1] = (double)(vessel.pos_y - y_offset);
      center[2] = (double)(vessel.pos_z - z_offset);
      spherefunc->SetCenter(center);
      sphere_radius = SPHERE_RADIUS; //CHECK
      spherefunc->SetRadius(sphere_radius);

      //Find Possible Matches from second volume inside sphere (TODO)
      QVector<vessel_node> match_nodes(0);
      foreach(vessel_node test_node,*low_nodes){
          SpherePointPosType test_point;
          test_point[0] = test_node.pos_x;
          test_point[1] = test_node.pos_y;
          test_point[2] = test_node.pos_z;
          if(spherefunc->Evaluate(test_point)) match_nodes.append(test_node);
        }

      if(match_nodes.size() != 0){

          //Find the distances for each match in a list
          //QVector<double> distances(0);
          QVector<double> disparities(0); //Define a similarity measure
          foreach(vessel_node match_candidate,match_nodes){
              double xx = match_candidate.pos_x-vessel.pos_x;
              double yy = match_candidate.pos_y-vessel.pos_y;
              //double zz = match_candidate.pos_z-vessel.pos_z;
              double distance = xx*xx + yy*yy;// + zz*zz;
              double radius_difference = abs(vessel.radius - match_candidate.radius);
              double disparity_measure = radius_difference*radius_difference +distance*distance*distance;
              disparities.append(disparity_measure);
            }

          //Find the Closest Point and assume its the match
          double min = *std::min_element(disparities.begin(), disparities.end());
          vessel_node result = match_nodes[disparities.indexOf(min)];
          QPair<int,int> pair;
          pair.first = vessel.node_num;
          pair.second = result.node_num;
          node_pairs.append(pair);
          disparities.clear();
        }
      match_nodes.clear();
    }
  return node_pairs;
}

//-------------------------------------------------------------------------------------//
//               FUNCTION : WRITE NODE PAIRS TO A FILE                         //
//------------------------------------------------------------------------------------//
void write_node_pairs(QList < QPair<int,int> > node_pairs,int offset){
  QFile file("node_pairs.txt");
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
      qDebug() << "Could not open the file for writing";
      return;
    }
  QTextStream out(&file);
  QPair<int,int> nodepair;
  foreach(nodepair,node_pairs){
      //QDataStream& operator << (&out,&nodepair);
      out << nodepair.first << "\t\t\t"
                               "" << (qint32)(nodepair.second+offset) << endl;
    }
  file.close();
}

//-------------------------------------------------------------------------------------//
//          FUNCTION : WRITE THE SWC OF STITCHED VOLUMES           //
//------------------------------------------------------------------------------------//
void write_stitched_swc(VesselBlock* volume, QString path){

  QString x = QString::number(volume->Vol_ID[0]);
  QString y = QString::number(volume->Vol_ID[1]);
  QString z = QString::number(volume->Vol_ID[2]);

  QString filename = path+"/Stitched_"+x+"_"+y+"_"+z+".swc";
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
      qDebug() << "Could not open the file for writing";
      return;
    }
  QTextStream out(&file);
  //Following lines added to see if it works with Vaa3D
  out << "#name MOST_Tracing"<< endl;
  out << "#comment" << endl;
  out << "#XYZ " << QString::number(volume->x_dim) << " " << QString::number(volume->y_dim) << " " << QString::number(volume->z_dim) << endl;
  out << "##n,type,x,y,z,radius,parent" << endl;
  //Now write all the vessel information
  foreach(vessel_node vessel, volume->vessels){
      out << display(&vessel) << endl;
    }
  file.close();
}


//-------------------------------------------------------------------------------------//
//          FUNCTION : WRITE THE SWC OF STITCHED VOLUMES           //
//------------------------------------------------------------------------------------//
void stitch(VesselBlock* vol_fixed, VesselBlock* vol_toappend,int direction,QString* Path) {
   //, double size_in_dir){

  //vol_id[i] -> 3 digit.
  //digit1 -> is it stitched (1=yes)
  //digit2->fixed block vol_id[i
  //digit3-> appendblock vol_id[i]

  VesselBlock stitched_volume; //TODO derive this from the block id of the two input volumes
  for(int i=0;i<3;i++) stitched_volume.Vol_ID[i] = vol_fixed->Vol_ID[i];
  stitched_volume.Vol_ID[direction-1] = 0;

  QList < QPair<int,int> > node_pairs;
  int x_offset = 0, y_offset = 0, z_offset = 0;
  double x_pos_shift = 0, y_pos_shift = 0, z_pos_shift = 0;
  switch(direction){
    case 1:
      x_offset = vol_fixed->x_dim;
      x_pos_shift = vol_toappend->x_dim;
      stitched_volume.x_dim = vol_fixed->x_dim + vol_toappend->x_dim;
      stitched_volume.y_dim = vol_fixed->y_dim;
      stitched_volume.z_dim = vol_fixed->z_dim;
      node_pairs = get_node_pairs(&(vol_fixed->terminals_xh),&(vol_toappend->terminals_xl),x_offset,y_offset,z_offset);
      break;
    case 2:
      y_offset = vol_fixed->y_dim;
      y_pos_shift = vol_toappend->y_dim;
      stitched_volume.x_dim = vol_fixed->x_dim;
      stitched_volume.y_dim = vol_fixed->y_dim + vol_toappend->y_dim;
      stitched_volume.z_dim = vol_fixed->z_dim;
      node_pairs = get_node_pairs(&(vol_fixed->terminals_yh),&(vol_toappend->terminals_yl),x_offset,y_offset,z_offset);
      break;
    case 3:
      z_offset = vol_fixed->z_dim;
      z_pos_shift = vol_toappend->z_dim;
      stitched_volume.x_dim = vol_fixed->x_dim;
      stitched_volume.y_dim = vol_fixed->y_dim;
      stitched_volume.z_dim = vol_fixed->z_dim + vol_toappend->z_dim;
      node_pairs = get_node_pairs(&(vol_fixed->terminals_zh),&(vol_toappend->terminals_zl),x_offset,y_offset,z_offset);
      break;
    }



//  //Actual Stitching
//  QPair<int,int> pair;
//  foreach(pair, node_pairs){
//      int vol2_nodenum =pair.second -vol_fixed->vessels.size();
//      if(vol_toappend->vessels[vol2_nodenum].parent == -1){
//          vol_toappend->vessels[vol2_nodenum].parent = pair.first;
//        }
//      else{// if(vol_fixed->vessels[pair.first].parent == -1){
//          vol_fixed->vessels[pair.first].parent = pair.second;
//        }
//    }


  foreach (vessel_node old_vessel,vol_fixed->vessels) {
      vessel_node new_vessel;
      new_vessel.node_num = old_vessel.node_num;
      new_vessel.parent = old_vessel.parent;
      new_vessel.pos_x = old_vessel.pos_x;
      new_vessel.pos_y = old_vessel.pos_y;
      new_vessel.pos_z = old_vessel.pos_z;
      new_vessel.radius = old_vessel.radius;
      new_vessel.type = old_vessel.type;
      stitched_volume.vessels.append(new_vessel);
    }

  foreach(vessel_node old_vessel, vol_toappend->vessels){
      vessel_node new_vessel;
      new_vessel.node_num = old_vessel.node_num +  vol_fixed->vessels.size();
      if(old_vessel.parent != -1) new_vessel.parent = old_vessel.parent + vol_fixed->vessels.size();
      else new_vessel.parent = -1;
      new_vessel.pos_x = old_vessel.pos_x + x_pos_shift;
      new_vessel.pos_y = old_vessel.pos_y + y_pos_shift;
      new_vessel.pos_z = old_vessel.pos_z + z_pos_shift;
      new_vessel.radius = old_vessel.radius;
      new_vessel.type = old_vessel.type;
      stitched_volume.vessels.append(new_vessel);
    }

  //Actual Stitching

  QPair<int,int> pair;
  int nodes_inserted = 1;
  foreach(pair, node_pairs){
      int toappend_index = (pair.second + vol_fixed->vessels.size() -1);
      if(stitched_volume.vessels[toappend_index].parent == -1){
          //CASE1:
          if(vol_fixed->vessels[pair.first - 1].parent == -1){
             // int nodes_inserted = 1;
              //Create a node parent that branches to the points to be connected in the two volumes
              vessel_node branch_parent;
              branch_parent.node_num = vol_fixed->vessels.size() + vol_toappend->vessels.size() + nodes_inserted;
              branch_parent.parent = -1;
              branch_parent.radius =(double)((stitched_volume.vessels[toappend_index].radius + stitched_volume.vessels[pair.first].radius)/2.0);
              branch_parent.pos_x = (double)((stitched_volume.vessels[toappend_index].pos_x + stitched_volume.vessels[pair.first].pos_x)/2.0);
              branch_parent.pos_y = (double)((stitched_volume.vessels[toappend_index].pos_y + stitched_volume.vessels[pair.first].pos_y)/2.0);
              branch_parent.pos_z = (double)((stitched_volume.vessels[toappend_index].pos_z + stitched_volume.vessels[pair.first].pos_z)/2.0);
              branch_parent.type = 7; //This is custom type of node. While parsing interpret as dummy connector & not branch
              //Connect the points
              stitched_volume.vessels[pair.first - 1].parent = branch_parent.node_num;
              stitched_volume.vessels[toappend_index].parent = branch_parent.node_num;
              stitched_volume.vessels.insert(branch_parent.node_num - 1,branch_parent);
              nodes_inserted +=1;
//              foreach(vessel_node vessel, stitched_volume.vessels){
//                  if(vessel.node_num > branch_parent.node_num){
//                      vessel.node_num += 1;
//                      if(vessel.parent != -1) vessel.parent +=1;
//                    }
//                }
//              stitched_volume.vessels[toappend_index+nodes_inserted].parent = branch_parent.node_num;
//              pair.second +=nodes_inserted;
            }
          //CASE2:
          else{
              stitched_volume.vessels[toappend_index].parent = pair.first;
            }
        }
      //CASE3:
      else if(vol_fixed->vessels[pair.first - 1].parent == -1){ //TBD
          stitched_volume.vessels[pair.first - 1].parent = toappend_index + 1;
        }
      //CASE4:
      else{
          vessel_node child1;
          child1.node_num = vol_fixed->vessels.size() + vol_toappend->vessels.size() + nodes_inserted;
          child1.parent = pair.first;
          child1.radius =(double)((stitched_volume.vessels[toappend_index].radius + stitched_volume.vessels[pair.first-1].radius)/2.0);
          child1.pos_x = (double)((stitched_volume.vessels[toappend_index].pos_x + stitched_volume.vessels[pair.first-1].pos_x)/2.0);
          child1.pos_y = (double)((stitched_volume.vessels[toappend_index].pos_y + stitched_volume.vessels[pair.first-1].pos_y)/2.0);
          child1.pos_z = (double)((stitched_volume.vessels[toappend_index].pos_z + stitched_volume.vessels[pair.first-1].pos_z)/2.0);
          child1.type = 7; //This is custom type of node. While parsing interpret as dummy connector & not branch
          nodes_inserted += 1;

          vessel_node child2;
          child2.node_num = vol_fixed->vessels.size() + vol_toappend->vessels.size() + nodes_inserted;
          child2.parent = toappend_index + 1;
          child2.type = 7;
          child2.radius = child1.radius;
          child2.pos_x = child1.pos_x;
          child2.pos_y = child1.pos_y;
          child2.pos_z = child1.pos_z;
          nodes_inserted += 1;

          stitched_volume.vessels.insert(child1.node_num - 1,child1);
          stitched_volume.vessels.insert(child2.node_num - 1,child2);
        }
    }

  write_node_pairs(node_pairs,vol_fixed->vessels.size());
  write_stitched_swc(&stitched_volume,* Path);
}


