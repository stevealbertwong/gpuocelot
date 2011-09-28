/*
 * DivergenceGraph.cpp
 *
 *  Created on: May 19, 2010
 *      Author: Diogo Sampaio
 */

// Ocelot Includes
#include <ocelot/analysis/interface/DivergenceGraph.h>

// Boost Includes
#include <boost/assign/list_of.hpp>

namespace analysis {

/*!\brief Clears the divergence graph */
void DivergenceGraph::clear(){
	DirectionalGraph::clear();
	_divergentNodes.clear();
	_specials.clear();
	_divergenceSources.clear();
	_upToDate = true;
}

/*!\brief Insert a special register source, possible source of divergence */
void DivergenceGraph::insertSpecialSource( const ir::PTXOperand* tid ){
	_upToDate = false;
	if( _specials.find(tid) == _specials.end() ){
		node_set a;
		_specials[tid] = a;
	}
}

/*!\brief Removes a special register source */
void DivergenceGraph::eraseSpecialSource( const ir::PTXOperand* tid ){
	_upToDate = false;
	_specials.erase(tid);
}

/*!\brief Define a node as being divergent,
	not depending on it's predecessors */
void DivergenceGraph::setAsDiv( const node_type &node ){
	if( nodes.find(node) != nodes.end() ){
		_upToDate = false;
		_divergenceSources.insert(node);
//		nodes.erase(node);
	}
}

/*!\brief Unset a node as being divergent, not depending on it's predecessors */
void DivergenceGraph::unsetAsDiv( const node_type &node ){
	if( _divergenceSources.find(node) != _divergenceSources.end() ){
		_upToDate = false;
//		nodes.insert(node);
		_divergenceSources.erase(node);
	}
}

/*!\brief Removes a node from the divergence graph */
bool DivergenceGraph::eraseNode( const node_type &nodeId ){
	_upToDate = false;
	_divergentNodes.erase(nodeId);
	return DirectionalGraph::eraseNode(nodeId);
}

/*!\brief Removes a node from the divergence graph */
bool DivergenceGraph::eraseNode( const node_iterator &node ){
	if( nodes.find(*node) == nodes.end() ){
		return false;
	}

	_upToDate = false;
	_divergentNodes.erase(*node);

	return DirectionalGraph::eraseNode(*node);
}

/*!\brief Inserts a edge[arrow] between two nodes of the graph
	/ Can create nodes if they don't exist */
int DivergenceGraph::insertEdge( const node_type &fromNode,
	const node_type &toNode, const bool createNewNodes ){
	_upToDate = false;
	return DirectionalGraph::insertEdge(fromNode, toNode, createNewNodes);
}

/*!\brief Inserts a edge[arrow] between two nodes of the graph
	/ Can create nodes if they don't exist */
int DivergenceGraph::insertEdge( const ir::PTXOperand* origin,
	const node_type &toNode, const bool createNewNodes ){
	if( createNewNodes ){
		insertSpecialSource(origin);
	}else if( _specials.find(origin) == _specials.end() ){
		return 1;
	}

	_specials[origin].insert(toNode);
	return 0;
}

/*!\brief Inserts a edge[arrow] between two nodes of the graph
	/ Can remove nodes if they are isolated */
int DivergenceGraph::eraseEdge( const node_type &fromNode,
	const node_type &toNode, const bool removeIsolatedNodes ){
	_upToDate = false;
	if( removeIsolatedNodes ){
		size_t actualNodesCount = nodes.size();
		int result = DirectionalGraph::eraseEdge(
			fromNode, toNode, removeIsolatedNodes);

		if( actualNodesCount == nodes.size() ){
			return result;
		}

		if( nodes.find(fromNode) == nodes.end() ){
			_divergentNodes.erase(fromNode);
		}else{
			_divergentNodes.erase(toNode);
		}

		return result;
	}

	return DirectionalGraph::eraseEdge(fromNode, toNode, removeIsolatedNodes);
}

/*!\brief Gests a list[set] of the divergent nodes */
const DirectionalGraph::node_set DivergenceGraph::getDivNodes() const{
	return _divergentNodes;
}

/*!\brief Tests if a node is divergent */
bool DivergenceGraph::isDivNode( const node_type &node ) const{
	return _divergentNodes.find(node) != _divergentNodes.end();
}

/*!\brief Tests if a node is a divergence source */
bool DivergenceGraph::isDivSource( const node_type &node ) const{
	return _divergenceSources.find(node) != _divergenceSources.end();
}

/*!\brief Tests if a special register is source of divergence */
bool DivergenceGraph::isDivSource( const ir::PTXOperand* srt ) const{
	return ((srt->addressMode == ir::PTXOperand::Special) && ( (srt->special == ir::PTXOperand::laneId) || (srt->special == ir::PTXOperand::tid)));
}

/*!\brief Tests if a special register is present on the graph */
bool DivergenceGraph::hasSpecial( const ir::PTXOperand* special ) const{
	return _specials.find(special) != _specials.end();
}

/*!\brief Gives the number of divergent nodes */
size_t DivergenceGraph::divNodesCount() const{
	return _divergentNodes.size();
}

/*!\brief Computes divergence spread
 * 1) Clear preview divergent nodes list
 * 2) Set all nodes that are directly dependent of a divergent
 	source {tidX, tidY, tidZ and laneid } as new divergent nodes
 * 3) Set all nodes that are explicitly defined as divergence
 	sources as new divergent nodes
 * 4) For each new divergent nodes
 * 4.1) Set all non divergent nodes that depend directly on the
 	divergent node as new divergent nodes
 * 4.1.1) Go to step 4 after step 4.3 until there are new divergent nodes
 * 4.2) Insert the node to the divergent nodes list
 * 4.3) Remove the node from the new divergent list
 */
void DivergenceGraph::computeDivergence(){
	if( _upToDate ){
		return;
	}
	/* 1) Clear preview divergent nodes list */
	_divergentNodes.clear();
	node_set newDivergenceNodes;

	/* 2) Set all nodes that are directly dependent of a divergent
		source {tidX, tidY, tidZ and laneid } as divergent */
	{
		std::map<const ir::PTXOperand*, node_set>::iterator
			divergence = _specials.begin();
		std::map<const ir::PTXOperand*, node_set>::iterator
			divergenceEnd = _specials.end();

		for( ; divergence != divergenceEnd; divergence++ ){
			if( isDivSource(divergence->first) ){
				const_node_iterator node = divergence->second.begin();
				const_node_iterator endNode = divergence->second.end();

				for( ; node != endNode; node++ ){
					newDivergenceNodes.insert(*node);
				}
			}
		}
	}
	{
		/* 3) Set all nodes that are explicitly defined as divergence
			sources as new divergent nodes */

		node_iterator divergence = _divergenceSources.begin();
		node_iterator divergenceEnd = _divergenceSources.end();

		for( ; divergence != divergenceEnd; divergence++ ){
			newDivergenceNodes.insert(*divergence);
		}
	}

	/* 4) For each new divergent nodes */
	while( newDivergenceNodes.size() != 0 ){
		node_type originNode = *newDivergenceNodes.begin();
		node_set newReachedNodes = getOutNodesSet(originNode);
		node_iterator current = newReachedNodes.begin();
		node_iterator last = newReachedNodes.end();

		/* 4.1) Set all non divergent nodes that depend directly on
			the divergent node as new divergent nodes */
		for( ; current != last; current++ ){
			if( !isDivNode(*current) ){
				/* 4.1.1) Go to step 4 after step 4.3 until there are
					new divergent nodes */
				newDivergenceNodes.insert(*current);
			}
		}

		/* 4.2) Insert the node to the divergent nodes list */
		_divergentNodes.insert(originNode);
		/* 4.3) Remove the node from the new divergent list */
		newDivergenceNodes.erase(originNode);
	}

	_upToDate = true;
}

/*!\brief Gives a string as name for a special register */
std::string DivergenceGraph::getSpecialName( const ir::PTXOperand* in ) const{
	assert (in->special < ir::PTXOperand::SpecialRegister_invalid);
	return (ir::PTXOperand::toString(in->special).erase(0, 1)
		+ ir::PTXOperand::toString(in->vIndex));
}

/*!\brief Prints the divergence graph in dot language */
std::ostream& DivergenceGraph::print( std::ostream& out ) const{
	using ir::PTXOperand;
	out << "digraph DivergentVariablesGraph{" << std::endl;

	/* Print divergence sources */
	std::map<const PTXOperand*, node_set>::const_iterator
		divergence = _specials.begin();
	std::map<const PTXOperand*, node_set>::const_iterator
		divergenceEnd = _specials.end();

	out << "//Divergence sources:" << std::endl;
	for( ; divergence != divergenceEnd; divergence++ ){
		if( divergence->second.size() ){
			out << getSpecialName(divergence->first)
				<< "[style=filled, fillcolor = \""
				<< (isDivSource(divergence->first)?"tomato":"lightblue")
				<< "\"]" << std::endl;
		}
	}

	/* Print nodes */
	out << "//Nodes:" << std::endl;
	const_node_iterator node = getBeginNode();
	const_node_iterator endNode = getEndNode();

	for( ; node != endNode; node++ ){
		out << *node << " [style=filled, fillcolor = \""
			<< (isDivNode(*node)?"lightyellow":"white") << "\"]" << std::endl;
	}

	out << std::endl;

	/* Print edges coming out of divergence sources */
	divergence = _specials.begin();
	divergenceEnd = _specials.end();

	out << "//Divergence out edges:" << std::endl;
	for( ; divergence != divergenceEnd; divergence++ ){
		if( divergence->second.size() ){
			node = divergence->second.begin();
			endNode = divergence->second.end();

			for( ; node != endNode; node++ ){
				out << getSpecialName(divergence->first) << "->"
					<< *node << "[color = \""
					<< (isDivSource(divergence->first)?"red":"blue")
					<< "\"]" << std::endl;
			}
		}
	}

	/* Print arrows between nodes */
	node = getBeginNode();
	endNode = getEndNode();

	out << "//Nodes edges:" << std::endl;
	for( ; node != endNode; node++ ){
		const node_set outArrows = getOutNodesSet(*node);
		const_node_iterator nodeOut = outArrows.begin();
		const_node_iterator endNodeOut = outArrows.end();

		for( ; nodeOut != endNodeOut; nodeOut++ ){
			out << *node << "->" << *nodeOut << std::endl;
		}
	}

	out << '}';

	return out;
}

std::ostream& operator<<( std::ostream& out, const DivergenceGraph& graph ){
	return graph.print(out);
}

}

